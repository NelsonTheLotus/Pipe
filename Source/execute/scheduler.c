#include "scheduler.h"

#include "../util/util.h"

#include <stdio.h>

void init_executors(unsigned int numJobs)
{
    //create a thread pool. Each thread has a corresponding single sequencial shell.
    if(numJobs == 0) numJobs = 1;

    char buff[100];
    sprintf(buff, "%u Runner(s) initialized! (placeholder)", numJobs);
    log_l(buff, INFO);
}


CommandResult runCommand(const ShellCommand command)
{
    char buff[256];
    sprintf(buff, "(placeholder) Running command: %s", command.command);
    log_l(buff, INFO);

    // pass command on to the appropriate thread (calculations required).
    // Thread will then manage the execution of the command with it's designated linked shell.

    return (CommandResult){0, 0, NULL, NULL};
}