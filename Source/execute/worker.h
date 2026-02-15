#pragma once
// A worker is a unit of execution.
// It pulls commands from lanes and decides what gets run how.
// It then issues those commands to shell, waiting for the output.

#include "shell.h"
#include <stddef.h>


#ifndef EXECUTE_PUBLIC


typedef struct {
    size_t id;
    Shell* executor;
} Worker;


Worker new_worker(size_t workerID);
void close_worker(Worker* worker);


#endif