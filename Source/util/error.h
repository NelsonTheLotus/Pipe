#pragma once

#include "../global.h"


// ==== External Structures ====

typedef enum ErrorLevel
{
    VERBOSE = 0,
    INFO = 1,
    WARNING = 2,
    CRITICAL = 3,
    FATAL = 4

} ErrorLevel;

typedef enum ErrorType
{
    NONE = 0,
    SYSTEM = 1,

} ErrorType;



// ==== Main Interface functions ====

void log_msg(const char* msg);    // Default lvl=INFO, typ = NONE
void log_lvl(const char* msg, ErrorLevel lvl);
void log_type(const char* msg, ErrorType type);
void log_error(const char* msg, ErrorLevel lvl, ErrorType type);



// ==== Parametering functions ====

void set_log_verbosity(ErrorLevel lvl);               // only logs >= lvl are kept/output
ErrorLevel get_log_verbosity(void);

bool log_enable_file(const char* path);               // open and enable file logging (returns success)
void log_disable_file(void);                          // close and disable file logging

void log_set_stdout_enabled(bool enabled);            // enable/disable printing to stdout (INFO/VERBOSE)
void log_set_stderr_enabled(bool enabled);            // enable/disable printing to stderr (WARNING/CRITICAL/FATAL)

void register_cleanup(void (*cleanup_cb)(void));     // register a cleanup function invoked on fatal
void log_flush(void);                                 // flush buffered logs to file / outputs
void log_shutdown(void);                              // flush + close logfile (does NOT exit)

/* Note:
 * log_error with FATAL will call the registered cleanup callback, flush buffers, close logfile and then exit(EXIT_FAILURE).
 * Use register_cleanup to register application-specific cleanup routines (freeing memory, closing sockets, etc).
 * register_cleanup returns a cleanup job ID (a simple unsigned int). Use this ID with void_cleanup(ID) to stop the 
 * cleanup job from running once exit is called. Use ID 0 to clear all cleanup jobs.
 */