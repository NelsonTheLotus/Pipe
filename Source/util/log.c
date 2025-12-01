#define LOG_INTERNAL    // Access private_functions
#include "log.h"
#undef LOG_INTERNAL     // Avoid conflicts with other defs

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



// ==== Internal structures ====

typedef struct LogEntry
{
    const char* msg;
    LogLevel level;
    LogType type;
    clock_t timestamp;
    struct LogEntry* nextEntry;

} LogEntry;

typedef struct
{
    FILE* logfile;          // File to log to

    bool logSupress;        // weather to supress logging to log file 
    bool logLevel;          // As of which level to log in file
    bool stdSupress;        // weather to supress logging to stdout
    bool stdLevel;          // As of which level to log to std
    
    LogEntry* stackTail;    // the tail of the log stack
    LogEntry* stackHead;    // The start of the log stack
    LogEntry* stackFile;    // Where the file logging begins
    
    bool fatal;             // weather a fatal error has occured
    // TODO: cleanup callback

} LogStack;


// ==== Static variables ====

// Static string mappings
static const char* const LS_verbose = "verbose";
static const char* const LS_info = "info";
static const char* const LS_warning = "warning";
static const char* const LS_critical = "critical";
static const char* const LS_fatal = "fatal";

static const char* const TS_none = "";
static const char* const TS_system = "system";
static const char* const TS_static_fallback = "static fallback";

// log stack
static LogStack mainStack = (LogStack){
    .logfile = NULL, 
    .logSupress = true, 
    .logLevel = INFO,
    .stdSupress = false,
    .stdLevel = WARNING,
    .stackTail = NULL,
    .stackHead = NULL,
    .stackFile = NULL,
    .fatal = false,
};

// static fallback logs for guaranteed logs in case of logging failure
static LogEntry fatal_malloc_log =
    {"Could not allocate required space while processing log.", FATAL, STATIC_FALLBACK, 0, NULL};



// ==== Internal Helpers ====

const char* level_to_str(LogLevel level)
{
    switch (level)
    {
        case VERBOSE: return LS_verbose;
        case INFO: return LS_info;
        case WARNING: return LS_warning;
        case CRITICAL: return LS_critical;
        case FATAL: return LS_fatal;
        default:
            return "VOID";
    }

    return "VOID";
}

const char* type_to_str(LogType type)
{
    switch (type)
    {
        case STATIC_FALLBACK: return TS_static_fallback;
        case NONE: return TS_none;
        case SYSTEM: return TS_system;
        default:
            return "unknown";
    }

    return "unknown";
}


//* Prints formatted output to desired outputs
void print_log(const LogEntry* log)
{
    if(log == NULL) return;

    // TODO: check if supress logging, log to file, log to std
    // TODO: Add colors to STD
    // TODO: improve timestamp printing
    printf("%ju - [%s] %s: %s\n", log->timestamp, level_to_str(log->level), type_to_str(log->type), log->msg);
    return;
}

//* Clear the log stack
void free_log_stack()
{
    LogEntry *currentEntry = mainStack.stackTail;
    LogEntry *previousEntry = NULL;
    while(currentEntry != NULL)
    {
        previousEntry = currentEntry;
        currentEntry = currentEntry->nextEntry;
        if(previousEntry->type = STATIC_FALLBACK) continue; // don't free static logs
        free(previousEntry);
    }
    return;
}

//* Run all cleanup callback functions
void cleanup(int EXITCODE)
{
    
    // TODO: run all cleanup routines
    
    log_disable_file();
    free_log_stack();
    if(EXITCODE) exit(EXITCODE);
    return;
}


// ==== Main Interface ====

//* Generates a new LogEntry and submits it.
void log_msg(const char* msg, LogLevel lvl, LogType type)
{
    LogEntry *newEntry = (LogEntry*)malloc(sizeof(LogEntry));

    // if new log could not be allocated: 
    //   - log a fatal malloc instead
    if(newEntry == NULL)
        newEntry = &fatal_malloc_log;
    else{
        newEntry->msg = msg;
        newEntry->level = lvl;
        newEntry->type = type;
        newEntry->nextEntry = NULL;
    }
    
    // set time of logging
    newEntry->timestamp = clock();

    // append new log
    if(mainStack.stackTail != NULL)
        mainStack.stackTail->nextEntry = newEntry;
    mainStack.stackTail = newEntry;

    // display log
    print_log(newEntry);
    return;
}
//TODO: Add more logging functions?

void close_logging()
{
    //TODO: Add non-fatal cleanup routines
    // => doCleanup()??
    cleanup(0);
}



// ==== Parametering Functions ====

void set_verbosity(LogLevel lvl)
{
    set_log_verbosity(lvl);
    set_std_verbosity(lvl);
}
void set_log_verbosity(LogLevel lvl)
{
    mainStack.logLevel = lvl;
    return;
}
LogLevel get_log_verbosity(void)
{
    return mainStack.logLevel;
}
void set_std_verbosity(LogLevel lvl)
{
    mainStack.stdLevel = lvl;
    return;
}
LogLevel get_std_verbosity(void)
{
    return mainStack.stdLevel;
}

bool log_enable_file(const char* path)
{
    mainStack.logSupress = false;
    return false; // TODO: implement
}

void log_disable_file(void)
{
    return; //TODO: implement
}

void std_set_enabled(bool enabled)
{
    mainStack.stdSupress = enabled;
}

unsigned int register_cleanup(void (*cleanup_cb)(void))
{
    //TODO: implement
    return 0;
}

bool remove_cleanup(unsigned int jobID)
{
    //TODO: implement
    return false;
}


// TODO: Log printing formating
// TODO: Cleanup callbacks stack manipulation
// TODO: Non-fatal cleanup + fatal cleanup
// TODO: Add file+stream logging
// TODO: Add more logging interface functions

