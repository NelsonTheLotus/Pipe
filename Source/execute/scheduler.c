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
        log_fatal("Could not allocate required space for workers. Stop.", SYSTEM);
        return;
    }
    for(size_t workerID = 0; workerID < numWorkers; workerID++)
    {
        trackers[workerID] = (WorkerTracker){workerID};
        init_worker(&trackers[workerID], &command_queue);
        run_worker(&trackers[workerID]);
    }
}


CommandResult runCommand(const ShellCommand command)
{
    char buff[256];
    sprintf(buff, "(placeholder) Running command: %s", command.command);
    log_l(buff, INFO);

    // pass command on to the appropriate thread (calculations required).
    // Thread will then manage the execution of the command with it's designated linked shell.

    return (CommandResult){0, 0, NULL, NULL};
}


void close_workers(void)
{
    if(trackers == NULL) return;
    for(size_t workerID = 0; workerID < numWorkers; workerID++)
    {
        stop_worker(&trackers[workerID], false);
        destroy_worker(&trackers[workerID]);
    }
    free(trackers);
}
