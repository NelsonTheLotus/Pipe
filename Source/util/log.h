#pragma once

#include "../global.h"


// ==== External Structures and types ====

typedef void (*callback_ptr)(void);

typedef enum LogLevel
{
    VERBOSE = 0,
    INFO = 1,
    WARNING = 2,
    CRITICAL = 3,
    FATAL = 4

} LogLevel;

typedef enum LogType
{
#ifdef LOG_INTERNAL         // Don't allow access unless it's log.c
    STATIC_FALLBACK = -1,    // Static fallback logs
#endif
    NONE = 0,
    SYSTEM = 1,

} LogType;



// ==== Main Interface functions ====

//* Create a new log
void log_msg(const char* msg, LogLevel lvl, LogType type);

//* Finnish logging gracefully
void close_logging();



// ==== Parametering functions ====

//* Set a global logging level
void set_verbosity(LogLevel lvl);
//* Supress any logs to file lower than <lvl>
void set_log_verbosity(LogLevel lvl);
LogLevel get_log_verbosity(void);
//* Supress any logs to stdout or stderr lower than <lvl>
void set_std_verbosity(LogLevel lvl);
LogLevel get_std_verbosity(void);

//* Enable logging to specified file -> success
bool log_enable_file(const char* path);
//* Write current log stack to file and close (if open) 
void log_disable_file(void);

//* Weather to enable prints to stdout and stderr
void std_set_enabled(bool enabled);

//* Register a job for the cleanup process -> job ID (1 indexed)
size_t register_cleanup(callback_ptr newCallback);
//* Remove job ID from cleanup list (1-indexed) -> success
bool remove_cleanup(size_t jobID);


/* Note:
 * log_error with FATAL will call the registered cleanup callbacks, flush buffers, close logfile and then exit(EXIT_FAILURE).
 * Use register_cleanup to register application-specific cleanup routines (freeing memory, closing sockets, etc).
 * register_cleanup returns a cleanup job ID (a simple unsigned int). Use this ID with void_cleanup(ID) to stop the 
 * cleanup job from running once exit is called. Use ID 0 to clear all cleanup jobs.
 */