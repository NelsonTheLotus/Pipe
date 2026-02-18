#pragma once

#include "../global.h"
#include <stdarg.h>


// ==== External Structures and types ====

typedef void (*callback_ptr)(void);

typedef enum LogLevel
{
    VERBOSE = 0,
    INFO = 1,
    WARNING = 2,
    CRITICAL = 3,
    FATAL = 4,

} LogLevel;

typedef enum LogSource
{
#ifdef LOG_INTERNAL         // Don't allow access unless it's log.c
    STATIC_FALLBACK = -1,    // Static fallback logs (in case of logging failures)
#endif
    NONE = 0,
    SYSTEM = 1,
    CLI = 2,
    LOGGER = 3,
    CACHE,

} LogSource;



// ==== Main Interface functions ====

//* Create a new log
void log_full(LogSource src, LogLevel lvl, const char* msg, ...);
//* log a message (INFO; NONE)
void log_msg(const char* msg, ...);
//* log a messgae with <lvl>
void log_l(LogLevel lvl, const char* msg, ...);
//* log a message with type <source>
void log_t(LogSource source, const char* msg, ...);
//* Log a fatal error
void log_fatal(LogSource source, const char* msg, ...);

//* Finnish non-fatal logging gracefully
void close_logging(void);



// ==== Parametering functions ====

//* Set a global logging level
void set_verbosity(LogLevel lvl);
//* Supress any logs to file lower than <lvl>
void set_log_verbosity(LogLevel lvl);
LogLevel get_log_verbosity(void);
//* Supress any logs to stdout or stderr lower than <lvl>
void set_std_verbosity(LogLevel lvl);
LogLevel get_std_verbosity(void);

//* Open a file for logging
bool set_log_file(const char* path, bool clearFile);
//* Write current log stack to file and close without cleanup.
void close_log_file(void);

//* Enable logging
void log_set_enabled(bool enabled);
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