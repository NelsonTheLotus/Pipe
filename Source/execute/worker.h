#pragma once
// A worker is a unit of execution.
// It pulls commands from lanes and decides what gets run how.
// It then issues those commands to shell, waiting for the output.

#include "shell.h"
#include "command.h"
#include "../global.h"

#include <stddef.h>
#include <pthread.h>
#include <stdatomic.h>


#ifndef EXECUTE_PUBLIC


typedef struct {
    unsigned int id;
    Shell executor;
    atomic_bool halt;   // graceful, finnish the queue
    CommandQueue* queue;
    // TODO: halt vs abort vs stop

    pthread_t thread_handle;
    pthread_mutex_t mutex_lock;
} WorkerTracker;


void init_worker(WorkerTracker* new_tracker, unsigned int worker_id);
bool run_worker(WorkerTracker* tracker);
void assign_queue(WorkerTracker* tracker, CommandQueue* new_queue);
void stop_worker(WorkerTracker* tracker, bool abort);   // TODO: stop vs halt vs abort vs shutdown
void destroy_worker(WorkerTracker* tracker);


#endif