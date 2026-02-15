#include "scheduler.h"

#include "worker.h"
#include <stdio.h>
#include <stdlib.h>
#include "../util/util.h"



// ==== Static worker references ====
static unsigned int numWorkers;
static Worker* workers = NULL;



void init_workers(unsigned int numJobs)
{
    //create a worker pool. Each worker has a corresponding single sequencial shell.
    if(numJobs == 0) numJobs = 1;
    numWorkers = numJobs;

    workers = (Worker*)malloc(numWorkers*sizeof(Worker));
    if(workers == NULL)
    {
        log_fatal("Could not allocate required workers. Stop.", SYSTEM);
        return;
    }
    for(size_t workerID = 0; workerID < numWorkers; workerID++)
        workers[workerID] = new_worker(workerID);

    // char buff[100];
    // sprintf(buff, "%u worker(s) initialized! (placeholder)", numWorkers);
    // log_l(buff, INFO);
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
    if(workers == NULL) return;
    for(size_t workerID = 0; workerID < numWorkers; workerID++)
    {
        close_worker(&(workers[workerID]));
    }
    free(workers);
}
