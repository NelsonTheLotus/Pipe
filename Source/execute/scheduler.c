#include "scheduler.h"

#include "../global.h"
#include "../util/util.h"
#include "worker.h"

#include <stdlib.h>
#include <pthread.h>



// MACROS 



// STATIC

static unsigned int worker_count = 0;
static Worker *worker_pool = NULL;



// INTERFACE

unsigned int open_workers(unsigned int num_workers)
{
    if(num_workers == 0) num_workers++;
    if(worker_pool != NULL) close_workers();
    else worker_pool = (Worker*)calloc(num_workers, sizeof(Worker));
    
    worker_count = 0;
    if(worker_pool == NULL)
    {
        log_l(CRITICAL, "Could not allocate space for workers.");
        return 0;   // error
    }

    for(unsigned int w_id = 0; w_id < num_workers; w_id++)
    {
        Worker *new_worker = &worker_pool[worker_count];

        worker_init(new_worker, w_id);
        if(!worker_start(new_worker))
            log_l(CRITICAL, "Failed to start new thread.");
        else worker_count++;
    }

    return worker_count;
}


void close_workers(void)
{
    for(unsigned int w_index = 0; w_index < worker_count; w_index++)
    {
        worker_request(&worker_pool[w_index], STOP);
        pthread_join(worker_pool[w_index].handle, NULL);
    }
}


void kill_workers(void)
{
    for(unsigned int w_index = 0; w_index < worker_count; w_index++)
        worker_kill(&worker_pool[w_index]);
}
// TODO: Log exit codes of sub-shells
// using the line: int shell_exit_code = worker_pool[w_index].executor.exit_code;   // exec struct is not destroyed. is still present
