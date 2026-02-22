#pragma once

#include "../global.h"

typedef struct command
{
    const char* cmd;
    const char* cwd;
    unsigned int timeout;
} Command;

typedef struct command_result
{
    bool has_returned;
    int return_code;
    char* return_text;
} CommandResult;