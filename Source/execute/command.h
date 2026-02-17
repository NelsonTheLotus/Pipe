#pragma once

#include "../global.h"

#include <pthread.h>



typedef struct 
{
    const char* command;
    const char* cwd;
    unsigned int timeout;
} ShellCommand;


typedef struct
{
    int exit_code;
    int signal;
    char *stdout_buff;
    char *stderr_buff;
} CommandResult;


#ifndef EXECUTE_PUBLIC

typedef struct
{
    ShellCommand commands;
    int head;
    int tail;
    int count;
    bool global_stop;
    // TODO: [global] halt vs abort vs stop


    pthread_mutex_t mutex_lock;
    pthread_cond_t not_full;    // allow main write
    pthread_cond_t not_empty;   // allow thread reads
} CommandQueue;

#endif


/* TODOs:
 * 1. Clean the structures used and make sure it's what we need
 * 2. Clear up the create process of said tracking structures (especially worker_tracker, creating vars in 3 different places)
 * 3. Passing error codes up the line and links
 * 4. stop vs halt vs abort clear up (local + global)
 * 5. Make the queue (it's currently very messy and not functional)
 * 6. Add queue lanes and some form of request from workers to scheduler
 * 7. Logging
 * 8. Passing down the commands and back up again.
*/