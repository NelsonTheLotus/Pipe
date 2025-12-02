#define LOG_INTERNAL    // Access private_functions
#include "log.h"
#undef LOG_INTERNAL     // Avoid conflicts with other defs

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



// ==== Internal structures and types ====

typedef struct LogEntry
{
    const char* msg;
    LogLevel level;
    LogSource source;
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

    callback_ptr* callbackList; // list of callback cleanup functions
    size_t callbackSize;
    size_t nextCallbackIndex;

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
static const char* const TS_cli = "cli";

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
    .callbackList = NULL,
    .callbackSize = 0,
    .nextCallbackIndex = 0,
};

// static fallback logs for guaranteed logs in case of logging failure
static LogEntry fatal_malloc_log =
    {"Could not allocate required space while processing log.", FATAL, STATIC_FALLBACK, 0, NULL};
static LogEntry fatal_job_alloc = 
    {"Could not reserve required space for cleanup job.", FATAL, STATIC_FALLBACK, 0, NULL};



// ==== Internal Helpers ====

const char* lvl_to_str(LogLevel lvl)
{
    switch (lvl)
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

const char* src_to_str(LogSource src)
{
    switch (src)
    {
        case STATIC_FALLBACK: return TS_static_fallback;
        case NONE: return TS_none;
        case SYSTEM: return TS_system;
        case CLI: return TS_cli;
        default:
            return "unknown";
    }

    return "unknown";
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
        if(previousEntry->source = STATIC_FALLBACK) continue; // don't free static logs
        free(previousEntry);
    }
    return;
}

//* Clear the job list
void free_callback_list()
{
    if(mainStack.callbackList != NULL)
        free(mainStack.callbackList);
    return;
}

//* Run all cleanup callback functions upon fatal
void cleanup()
{
    
    for(size_t jobID = 0; jobID < mainStack.callbackSize; jobID++)
    {
        callback_ptr cleanupFn = mainStack.callbackList[jobID];
        if(cleanupFn == NULL)
            continue;
        cleanupFn();  // call cleanup fn
    }
    
    close_logging();
    exit(-1);
}

//* Prints formatted output to desired outputs
void print_log(const LogEntry* log)
{
    if(log == NULL) return;

    // TODO: check if supress logging, log to file, log to std
    // TODO: Add colors to STD
    // TODO: improve timestamp printing
    printf("%ju - [%s] %s: %s\n", log->timestamp, lvl_to_str(log->level), src_to_str(log->source), log->msg);
    return;
}

//* Shchedule a new log from entry
void log_entry(LogEntry* logEntry)
{
    // Set time
    logEntry->timestamp = clock();

    // append new log
    if(mainStack.stackTail != NULL)
        mainStack.stackTail->nextEntry = logEntry;
    mainStack.stackTail = logEntry;

    // display log
    print_log((const LogEntry*)logEntry);
    if(logEntry->level == FATAL)
        cleanup();

    return;
}



// ==== Main Interface ====

void log_full(const char* msg, LogLevel lvl, LogSource src)
{
    LogEntry *newEntry = (LogEntry*)malloc(sizeof(LogEntry));

    // if new log could not be allocated: 
    //   - log a fatal malloc instead
    if(newEntry == NULL)
    {
        log_entry(&fatal_malloc_log);
        return;
    }

    newEntry->msg = msg;
    newEntry->level = lvl;
    newEntry->source = src;
    newEntry->nextEntry = NULL;
    
    log_entry(newEntry);
    return;
}

void log_msg(const char* msg)
{
    log_full(msg, INFO, NONE);
    return;
}

void log_l(const char* msg, LogLevel lvl)
{
    log_full(msg, lvl, NONE);
    return;
}

void log_t(const char* msg, LogSource src)
{
    log_full(msg, INFO, src);
    return;
}

void log_fatal(const char* msg, LogSource src)
{
    log_full(msg, FATAL, src);
    return;
}

void close_logging()
{
    log_disable_file();
    free_log_stack();
    free_callback_list();
    return;
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

size_t register_cleanup(callback_ptr newCallback)
{
    if(mainStack.callbackList == NULL)
    {
        mainStack.callbackList = (callback_ptr*)malloc(sizeof(callback_ptr));
        mainStack.callbackSize = 1;
    }
    if(mainStack.nextCallbackIndex >= mainStack.callbackSize)
    {
        mainStack.callbackSize++;
        callback_ptr* newCallbackList =     // don't assign to avoid segfaults when doing fatal cleanup
            (callback_ptr*)realloc(mainStack.callbackList, mainStack.callbackSize*sizeof(callback_ptr));

        if(newCallbackList == NULL)
        {
            newCallback();  // do the cleanup fn that was not allocated and will hence not be run
            log_entry(&fatal_job_alloc);
            return 0;       // 0 is error
        }
    }

    mainStack.callbackList[mainStack.nextCallbackIndex] = newCallback;
    size_t jobID = mainStack.nextCallbackIndex+1; // transform to 1-indexed
    while(++(mainStack.nextCallbackIndex) < mainStack.callbackSize)  // search for next spot as of the index
    {
        if(mainStack.callbackList[mainStack.nextCallbackIndex] == NULL)    // spot found
            break;
    }

    return jobID;
}

bool remove_cleanup(size_t jobID)
{
    if(jobID > mainStack.callbackSize)
        return false;   // no job has this ID
    
    mainStack.callbackList[--jobID] = NULL; // jobID is 1-indexed
    if(mainStack.nextCallbackIndex > jobID)
        mainStack.nextCallbackIndex = jobID;    // backup the search for available spots

    return true;
}


// TODO: Log printing formating --------------- 
// TODO: Cleanup callbacks stack manipulation - (DONE)
// TODO: Non-fatal cleanup + fatal cleanup ---- (DONE)
// TODO: Add file+stream logging --------------
// TODO: Add more logging interface functions - (DONE)

