#pragma once

#include "../global.h"


// Full public facing API

typedef enum SystemType
{
    UNKNOWN,
    WINDOWS,
    UNIX,
    MACOS,
    BSD,

} SystemType;

//* Create a directory and parent directories from cwd 
bool make_dir(const char* path);

//* get current working directory
const char* get_cwd(void);
//* set current working directory
bool set_cwd(const char* path);

//* get the current system type
SystemType get_system(void);