#include "worker.h"


#include "shell.h"
#include "../util/util.h"

#include <stddef.h>
#include <pthread.h>
#include <stdio.h>


// local
static void* worker_loop(void* arg)
{
    WorkerTracker* tracker = arg;
    if(tracker == NULL) return NULL;

    pthread_mutex_lock(&tracker->mutex_lock);

    tracker->executor = new_shell();
    printf("Worker %d is running.\n", tracker->id);
    int retCode = stop_shell(&tracker->executor, false);

    pthread_mutex_unlock(&tracker->mutex_lock);
    // return &retCode;
    return NULL;
    // TODO: store and return ret code properly
}


// runs in main thread, no need for locks
void init_worker(WorkerTracker* new_tracker, unsigned int worker_id)
{
    if(new_tracker == NULL) return;
    
    new_tracker->id = worker_id;
    new_tracker->executor = (Shell){-1};
    new_tracker->halt = true;   // default no-run
    //new_tracker->abort = false;   // default no-run
    new_tracker->queue = NULL;

    pthread_mutex_init(&new_tracker->mutex_lock, NULL);
    return;
}


bool run_worker(WorkerTracker* tracker)
{
    if(tracker == NULL) return false;
    tracker->halt = false;

    pthread_mutex_lock(&tracker->mutex_lock); // allow thread handle to be written
    pthread_create(&tracker->thread_handle, NULL, worker_loop, tracker);
    pthread_mutex_unlock(&tracker->mutex_lock);

    return true;
}


void assign_queue(WorkerTracker* tracker, CommandQueue* new_queue)
{
    pthread_mutex_lock(&tracker->mutex_lock);
    tracker->queue = new_queue;
    pthread_mutex_unlock(&tracker->mutex_lock);
    return;
}


void stop_worker(WorkerTracker* tracker, bool abort)
{
    if(tracker == NULL) return;

    atomic_store(&tracker->halt, true);
    //if(abort) atomic_store(&tracker->abort, true);
    int thread_return;
    pthread_join(tracker->thread_handle, NULL);
    printf("Worker thread closed.\n");
    // TODO: timeout + cancel thread
    // TODO: fetch thread exit codef properly

    return;
}

void destroy_worker(WorkerTracker* tracker)
{
    pthread_mutex_destroy(&tracker->mutex_lock);
    return;
}

// TODO: lane requesting

// WorkerTracker new_worker(size_t workerID)
// {
//     Shell newShell = new_shell();
//     if(newShell.shell_pid == -1)
//         log_l("Failed to create executor shell.", CRITICAL);    // TODO: add more log info on success/failure (pid, errcode, ...)
//     else log_l("Created new shell.", INFO);
//     return (WorkerTracker){workerID, newShell};
// }


// void close_worker(WorkerTracker* worker)
// {
//     stop_shell(&(worker->executor), false);
//     log_l("Shell terminated.", INFO);   // TODO: add more info on which chell was terminated
// }