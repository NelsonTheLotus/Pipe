#pragma once
// internal only, do not include elsewhere

#include "command.h"
#include "shell.h"

#include <pthread.h>

typedef enum state
{
    INIT = 0,       // initialized but not running
    RESETTING = 1,  // restarting shell and thread
    RUNNING = 2,    // Currently pulling and running
    WAITING = 3,    // Waiting for jobs
    HALTED = 4,     // Idle, waiting for start (shell active)
    STOPPED = 5,    // Idle, waiting for start (shell inactive)
    DEAD = 6        // Thread is no longer alive
} worker_state;


typedef enum action
{
    CONTINUE = 0,
    RESET = 1,
    HALT = 2,
    STOP = 3,
} worker_action;


typedef struct worker
{
    unsigned int worker_id;

    Shell executor;
    worker_state state;
    worker_action action;

    pthread_t handle;
    pthread_mutex_t mutex_lock;
    pthread_cond_t flag;    
} Worker;


// Worker actions
bool worker_init(Worker* w, unsigned int id);
bool worker_start(Worker* w);   // Put into running
void worker_kill(Worker* w);    // aborts current job and joins thread

bool worker_request(Worker* w, worker_action action);


/* Worker states and actions
 * States:
 * - INIT: The worker has been initialized and is ready to run.
 * - RESET: The worker is currently resetting as if newly called with the same params.
 * - RUNNING: The worker is running jobs or pulling the m from the queue
 * - WAITING: The worker is active, waiting a queue input
 * - HALTED: The worker is halted, but can be reactivated
 * - STOPPED: The worker is no longer active, but can be restarted.
 * - DEAD: The worker has been killed, it can no longer start.
 * 
 * function actions:
 * - init worker: Create a new worker instance
 * - start worker: Put selected worker into running mode
 * - kill worker: Abort current job and terminate running thread.
 * 
 * Signal actions:
 * - RESET: Reset the worker to as new (reset is soft, no abortion), ends up in the RESET state, start must be called.
 * - HALT: Halt after current job is done, shell is not destroyed.
 * - STOP: Stop after current job is done running and terminate shell
 */