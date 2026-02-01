#pragma once


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
    char *stdout;
    char *stderr;
} CommandResult;


void init_executors(unsigned int numJobs);
CommandResult runCommand(const ShellCommand command);