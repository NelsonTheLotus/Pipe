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
    size_t id;
    Shell executor;
    atomic_bool halt;   // graceful, finnish the queue
    atomic_bool abort;  // forced, stop immediatly
    CommandQueue* queue;
    // TODO: halt vs abort vs stop

    pthread_t thread_handle;
    pthread_mutex_t mutex_lock;
} WorkerTracker;


void init_worker(WorkerTracker* new_tracker, CommandQueue* command_queue);
bool run_worker(WorkerTracker* tracker);
void close_worker(WorkerTracker* worker_tracker, bool abort);


#endif