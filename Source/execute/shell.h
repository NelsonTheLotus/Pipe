#pragma once

#include "command.h"

#include <unistd.h>


typedef struct shell
{
    pid_t pid;  // pid == -1 -> not running
    int input_pipe;
    int output_pipe;

    bool halted;
    int exit_code;
} Shell;


// Shell control
Shell shell_init();             // creates a new shell struct
bool shell_start(Shell* s);
void shell_halt(Shell* s);
void shell_resume(Shell* s);
bool shell_stop(Shell* s, unsigned int timeout); // gracefull stop; timeout(0) -> sigterm, ret false
void shell_kill(Shell* s, int timeout); // forceful termination; timeout(0) -> sigkill
