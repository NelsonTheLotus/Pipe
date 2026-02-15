#pragma once
// Scheduler manages the input command stream and
// dispatches the commands to the appropriate lanes (queues)
// from which workers will pull. It listens for errors in workers
// and halts if a fatal error occured.d


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


void init_executors(unsigned int numJobs);
CommandResult runCommand(const ShellCommand command);