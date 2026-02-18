#define LOG_INTERNAL    // Access private functions
#include "log.h"
#undef LOG_INTERNAL     // Avoid conflicts with other defs

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



// ==== Internal structures and types ====

typedef enum ColorType
{
    CLR_VERBOSE = 0,    // = VERBOSE
    CLR_INFO = 1,       // = INFO
    CLR_WARNING = 2,    // = WARNING
    CLR_CRITICAL = 3,   // = CRITICAL
    CLR_FATAL = 4,      // = FATAL
    CLR_CLEAR = 5,
    CLR_TIME = 6,
    CLR_SOURCE = 7,
    CLR_MSG = 8,

} ColorType;

typedef struct LogEntry
{
    char* msg;
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

// Static colors array
static const char* colors[] = 
{
    "\x1b[1;34m",   // CLR_VERBOSE  = Bold blue
    "\x1b[1;32m",   // CLR_INFO     = Bold green
    "\x1b[1;33m",   // CLR_WARNING  = Bold yellow
    "\x1b[1;35m",   // CLR_CRITICAL = Bold purple
    "\x1b[1;31m",   // CLR_FATAL    = Bold red
    "\x1b[0m",      // CLR_CLEAR    = clear
    "\x1b[0;36m",   // CLR_TIME     = cyan
    "\x1b[0;97m",   // CLR_SOURCE   = normal white (high-intensity)
    "\x1b[0;90m",   // CLR_MSG      = light black (high-intensity)
};

// Static string mappings
static const char* const LS_verbose = "VERBOSE";
static const char* const LS_info = "INFO";
static const char* const LS_warning = "WARNING";
static const char* const LS_critical = "CRITICAL";
static const char* const LS_fatal = "FATAL";

static const char* const TS_none = "";
static const char* const TS_system = "system";
static const char* const TS_static_fallback = "static fallback";
static const char* const TS_cli = "cli";
static const char* const TS_logger = "logger";

// log stack
static LogStack mainStack = (LogStack)
{
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

// static fallback logs for guaranteed logs in case of logging failure (MUST BE FATAL OR SEGFAULT)
static LogEntry fatal_malloc_log =
    {"Could not allocate required space while processing log.", FATAL, STATIC_FALLBACK, 0, NULL};
static LogEntry fatal_job_alloc = 
    {"Could not reserve required space for cleanup job.", FATAL, STATIC_FALLBACK, 0, NULL};



// ==== Internal Helpers ====

static const char* lvl_to_str(LogLevel lvl)
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

static const char* src_to_str(LogSource src)
{
    switch (src)
    {
        case STATIC_FALLBACK: return TS_static_fallback;
        case NONE: return TS_none;
        case SYSTEM: return TS_system;
        case CLI: return TS_cli;
        case LOGGER: return TS_logger;
        default:
            return "unknown";
    }

    return "unknown";
}


//* Reduce log_stack up to boundary
static void clean_log_stack(LogEntry* boundary)
{
    LogEntry *currentEntry = mainStack.stackHead;
    LogEntry *previousEntry = NULL;
    while(currentEntry != boundary)
    {
        if(currentEntry == NULL) break; // end reached

        previousEntry = currentEntry;
        currentEntry = currentEntry->nextEntry;
        if(previousEntry->source = STATIC_FALLBACK) continue; // don't free static logs

        free(previousEntry->msg);
        free(previousEntry);
    }

    mainStack.stackHead = boundary;
    return;
}

//* Clear the log stack
static void clear_log_stack()
{
    clean_log_stack(NULL);
    return;
}

//* Clear the job list
static void free_callback_list()
{
    if(mainStack.callbackList != NULL)
        free(mainStack.callbackList);
    return;
}

//* Run all cleanup callback functions upon fatal
static void cleanup()
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
static void print_log(const LogEntry* log)
{
    if(log == NULL) return;
    if(mainStack.stdSupress) return;    // don't print if supressing std
    if(log->level < mainStack.stdLevel) return; // don't print if not important

    FILE *outStream = stdout;
    if(log->level >= CRITICAL)  // log to stderr if critical or fatal
        outStream = stderr;

    fprintf(outStream, "%s%ju%s - [%s%s%s] %s%s%s: %s%s%s\n", colors[CLR_TIME], log->timestamp, colors[CLR_CLEAR],
                                                              colors[log->level], lvl_to_str(log->level), colors[CLR_CLEAR],
                                                              colors[CLR_SOURCE], src_to_str(log->source), colors[CLR_CLEAR],
                                                              colors[CLR_MSG], log->msg, colors[CLR_CLEAR]);
                                                              
    // TODO: improve timestamp printing
    return;
}

//* Shchedule a new log from entry
static void log_entry(LogEntry* logEntry)
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

static void create_log(LogSource src, LogLevel lvl, const char* msg, va_list msg_args)
{
    LogEntry *newEntry = (LogEntry*)malloc(sizeof(LogEntry));

    // if new log could not be allocated: 
    //   - log a fatal malloc instead
    if(newEntry == NULL)
    {
        log_entry(&fatal_malloc_log);
        return;
    }

    size_t buf_size = strlen(msg)*sizeof(char);
    char *msg_buf = (char*)malloc(buf_size);     // initial guess
    if(msg_buf == NULL) log_entry(&fatal_malloc_log);

    va_list args_copy;  // in case of vsnprintf failure
    va_copy(args_copy, msg_args);
    int needed = vsnprintf(msg_buf, buf_size, msg, args_copy);
    if(needed >= buf_size)
    {
        buf_size = needed*sizeof(char);
        msg_buf = realloc(msg_buf, buf_size);
        if(msg_buf == NULL) log_entry(&fatal_malloc_log);
        vsnprintf(msg_buf, buf_size, msg, msg_args);
    }

    newEntry->msg = msg_buf;
    newEntry->level = lvl;
    newEntry->source = src;
    newEntry->nextEntry = NULL;

    log_entry(newEntry);
    return;
}



// ==== Main Interface ====

void log_full(LogSource src, LogLevel lvl, const char* msg, ...)
{
    va_list msg_args;
    va_start(msg_args, msg);
    create_log(src, lvl, msg, msg_args);
    va_end(msg_args);
    return;
}

void log_msg(const char* msg, ...)
{
    va_list msg_args;
    va_start(msg_args, msg);
    create_log(NONE, INFO, msg, msg_args);
    va_end(msg_args);
    return;
}

void log_l(LogLevel lvl, const char* msg, ...)
{
    va_list msg_args;
    va_start(msg_args, msg);
    create_log(NONE, lvl, msg, msg_args);
    va_end(msg_args);
    return;
}

void log_t(LogSource src, const char* msg, ...)
{
    va_list msg_args;
    va_start(msg_args, msg);
    create_log(src, INFO, msg, msg_args);
    va_end(msg_args);
    return;
}

void log_fatal(LogSource src, const char* msg, ...)
{
    va_list msg_args;
    va_start(msg_args, msg);
    create_log(src, FATAL, msg, msg_args);
    va_end(msg_args);
    return;
}

void close_logging(void)
{
    // don't run cleanup functions
    close_log_file();
    clear_log_stack();
    free_callback_list();
    return;
}

void write_file_logs(void)
{
    if(mainStack.logfile == NULL) return;   // don't write if no file
    if(mainStack.logSupress) return;        // don't write if supressing logs

    while(mainStack.stackFile != NULL)
    {
        if(mainStack.stackFile->level < mainStack.logLevel) continue;
        fprintf(mainStack.logfile, "%s\n", mainStack.stackFile->msg);
        mainStack.stackFile = mainStack.stackFile->nextEntry;
    }

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
    write_file_logs();  // Write all logs up to now
    mainStack.logLevel = lvl;
    mainStack.stackFile = mainStack.stackTail;
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

bool set_log_file(const char* path, bool clearFile)
{
    if(clearFile)
        mainStack.logfile = fopen(path, "w");
    else mainStack.logfile = fopen(path, "a");
    if(mainStack.logfile == NULL)
    {
        log_full(LOGGER, CRITICAL, "Could not open log file: \"%s\"", path);
        log_set_enabled(false);
        return false;
    }

    log_set_enabled(true);
    return true;
}
void close_log_file(void)
{
    log_set_enabled(false);     // writes all logs to file
    if(mainStack.logfile == NULL) return; // job is already done

    fclose(mainStack.logfile);
    mainStack.logfile = NULL;
    return;
}

void log_set_enabled(bool enabled)
{
    mainStack.logSupress = !enabled;
    if(enabled)
        mainStack.stackFile = mainStack.stackTail;
    else    // supress = true
    {
        if(mainStack.logfile == NULL)
            log_full(LOGGER, WARNING, "No log file to write to.");
        write_file_logs();
        mainStack.stackFile = NULL;
    }

    return;
}
void std_set_enabled(bool enabled)
{
    mainStack.stdSupress = !enabled;
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


// TODO: Log printing formating --------------- (TIME)
// TODO: Cleanup callbacks stack manipulation - (DONE)
// TODO: Non-fatal cleanup + fatal cleanup ---- (DONE)
// TODO: Add file+stream logging -------------- (DONE)
// TODO: Add more logging interface functions - (DONE)

