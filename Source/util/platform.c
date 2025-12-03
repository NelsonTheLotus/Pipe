#include "platform.h"


// ==== Static Elements ====

static const char* SystemNames =
{
    "Unknown",
    "Windows",
    "POSIX",
    "Mac-OS",
    "BSD-like",
};

#define SYSTEM_TYPE UNKNONW

#ifdef _WIN32
#define SYSTEM_TYPE WINDOWS
#endif

#ifdef __unix__
#define SYSTEM_TYPE UNIX
#endif

#ifdef 



// ==== Interface ====

bool make_dir(const char* path);

const char* get_cwd(void);
bool set_cwd(const char* path);

SystemType get_system(void);
