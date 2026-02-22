#pragma once

#include "command.h"


// controls
unsigned int open_workers(unsigned int num_workers);    // returns the number of workers opened
void close_workers(void);   // graceful closing
void kill_workers(void);    // forceful closing

// manipulations
CommandResult* schedule_command(Command cmd);   // TODO: queue + lanes 