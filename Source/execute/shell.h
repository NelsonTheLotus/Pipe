#pragma once
// Shell is the executor. It takes a command,
// issues it through pipes, waits for the output
// and returns the result back to it's linked worker.

#include "../global.h"
#include "command.h"
#include <unistd.h>


#define GRACEFUL_TIMEOUT 2
#define FORCEFUL_TIMEOUT 2


#ifndef EXECUTE_PUBLIC

typedef struct {
    pid_t shell_pid;
    int shell_input;
    int shell_output;

    char read_buff[256];
    int err_code;
} Shell;


Shell new_shell(void);
int stop_shell(Shell* shell, bool force);
CommandResult issue_command(Shell* shell, const char* command);

#endif