#pragma once
// Scheduler manages the input command stream and
// dispatches the commands to the appropriate lanes (queues)
// from which workers will pull. It listens for errors in workers
// and halts if a fatal error occured.d

#include "command.h"


void init_workers(unsigned int numJobs);
CommandResult runCommand(const ShellCommand command);
void close_workers(void);