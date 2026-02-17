#pragma once
// Scheduler manages the input command stream and
// dispatches the commands to the appropriate lanes (queues)
// from which workers will pull. It listens for errors in workers
// and halts if a fatal error occured.d

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



typedef struct
{
    ShellCommand commands;
    int head;
    int tail;
    int count;

    pthread_mutex_t mutex;
    pthread_cond_t not_full;    // allow main write
    pthread_cond_t not_empty;   // allow thread reads
} CommandQueue;


void init_workers(unsigned int numJobs);
CommandResult runCommand(const ShellCommand command);
void close_workers(void);