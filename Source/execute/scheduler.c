#include "scheduler.h"

#include "worker.h"
#include <stdio.h>
#include <stdlib.h>
#include "../util/util.h"



// ==== Static worker references ====
static unsigned int numWorkers;
static WorkerTracker* trackers = NULL;
static CommandQueue command_queue;


void init_workers(unsigned int numJobs)
{
    // Setup queue
    pthread_mutex_init(&command_queue.mutex_lock, NULL);
    pthread_cond_init(&command_queue.not_full, NULL);
    pthread_cond_init(&command_queue.not_empty, NULL);

    if(numJobs == 0) numJobs = 1;
    numWorkers = numJobs;

    trackers = (WorkerTracker*)malloc(numWorkers*sizeof(WorkerTracker));
    if(trackers == NULL)
    {
        log_fatal(SYSTEM, "Could not allocate required space for workers. Stop.");
        return;
    }
    for(unsigned int workerID = 0; workerID < numWorkers; workerID++)
    {
        init_worker(&trackers[workerID], workerID);
        assign_queue(&trackers[workerID], &command_queue);
        bool success = run_worker(&trackers[workerID]);
        if(success) log_msg("Thread %u initialized.", trackers[workerID].id);
    }
}


CommandResult runCommand(const ShellCommand command)
{
    // char buff[256];
    // sprintf(buff, "(placeholder) Running command: %s", command.cmd);
    
    Shell* executing_shell = &trackers[0].executor;
    ShellCommand cmd = {"__terminate_cmd_1", "pwd", "/", 0};
    //log_l(INFO, "(placeholder) Running command: %s with id: %s", cmd.cmd, cmd.id_glob);

    if(executing_shell->shell_pid != -1)
    {
        issue_command(executing_shell, cmd);
    }
    else log_l(CRITICAL, "Shell closed; could not run.");
    return "Call issue command through thread only, it knows if the shell is running and will issue itself"
    "also prevents race conditions on issuing command and closing thread (ifcond above is not atomic or thread-safe.)"
    
    // pass command on to the appropriate thread (calculations required).
    // Thread will then manage the execution of the command with it's designated linked shell.

    return (CommandResult){0, 0, NULL};
}


void close_workers(void)
{
    if(trackers == NULL) return;
    for(unsigned int workerID = 0; workerID < numWorkers; workerID++)
    {
        stop_worker(&trackers[workerID], false);
        destroy_worker(&trackers[workerID]);
        log_msg("Thread %u destroyed.", trackers[workerID].id);
    }
    free(trackers);
}
