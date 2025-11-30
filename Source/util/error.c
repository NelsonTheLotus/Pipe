#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



// ==== Internal structures ====

typedef struct
{
    const char* msg;
    ErrorLevel level;
    ErrorType type;
    time_t timestamp;
    struct Error* nextError;

} LogEntry;

typedef struct
{
    FILE* logfile;      // File to log to

    bool supressLog;    // weather to supress logging to log file 
    bool supressOut;    // weather to supress logging to stdout
    bool supressErr;    // weather to supress logging to stderr
    bool verbose;       // weather to display verbose
    bool fatal;         // weather a fatal error has occured

    LogEntry* stackTail;
    LogEntry* stackHead;

    // TODO: cleanup callback

} LogStack;


// ==== Static variables ====

// Static string mappings
static const char* const LS_verbose = "verbose";
static const char* const LS_info = "info";
static const char* const LS_warning = "warning";
static const char* const LS_critical = "critical";
static const char* const LS_fatal = "fatal";

static const char* const TS_none = NULL;
static const char* const TS_system = "system";

// log satck
static LogStack mainStack = (LogStack){
    .logfile = NULL, 
    .supressLog = true, 
    .supressOut = false,
    .supressErr = false,
    .verbose = false,
    .fatal = false,
    .stackTail = NULL,
    .stackHead = NULL};

// unnallocated errors for guaranteed failure in case of error failure
static const LogEntry error_malloc = 
    {"Could not allocate required space while processing log.", FATAL, SYSTEM, NULL};



// ==== Internal Helpers ====

const char* level_to_str(ErrorLevel level)
{
    switch (level)
    {
        case VERBOSE: return LS_verbose;
        case INFO: return LS_info;
        case WARNING: return LS_warning;
        case CRITICAL: return LS_critical;
        case FATAL: return LS_fatal;
        default:
            return NULL;
    }

    return NULL;
}

const char* type_to_str(ErrorType type)
{
    switch (type)
    {
    case NONE: return TS_none;
    case SYSTEM: return TS_system;
    default:
        return NULL;
    }

    return NULL;
}



// ==== Main Interface ====
void log_error(const char* msg, ErrorLevel lvl, ErrorType type)
{
    // TODO: implement
}

void log_msg(const char* msg)     // Default lvl=INFO, typ = NONE
{
    log_error(msg, INFO, NONE);
    return;
}

void log_lvl(const char* msg, ErrorLevel lvl)
{
    log_error(msg, lvl, NONE);
    return;
}

void log_type(const char* msg, ErrorType type)
{
    log_error(msg, INFO, type);
}



// ==== Parametering Functions ====

// TODO: implement all
void set_log_verbosity(ErrorLevel lvl);               // only logs >= lvl are kept/output
ErrorLevel get_log_verbosity(void);

bool log_enable_file(const char* path);               // open and enable file logging (returns success)
void log_disable_file(void);                          // close and disable file logging

void log_set_stdout_enabled(bool enabled);            // enable/disable printing to stdout (INFO/VERBOSE)
void log_set_stderr_enabled(bool enabled);            // enable/disable printing to stderr (WARNING/CRITICAL/FATAL)

void register_cleanup(void (*cleanup_cb)(void));     // register a cleanup function invoked on fatal
void log_flush(void);                                 // flush buffered logs to file / outputs
void log_shutdown(void);                              // flush + close logfile (does NOT exit)


/* From our dear all-knowing overlord
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------------------------
   Internal types & structures
   --------------------------- *

typedef struct LogEntry
{
    char* msg;               /* owned copy *
    ErrorLevel level;
    ErrorType type;
    time_t timestamp;
    struct LogEntry* next;
} LogEntry;

typedef struct
{
    FILE* logfile;

    bool supressLog;    /* whether to suppress writing to log file *
    bool supressOut;    /* whether to suppress writing to stdout *
    bool supressErr;    /* whether to suppress writing to stderr *

    ErrorLevel verbosity; /* minimum level to keep/output *

    bool fatal;            /* whether a fatal error occurred *

    LogEntry* head;        /* oldest entry *
    LogEntry* tail;        /* newest entry *

    void (*cleanup_cb)(void);

} LogStack;

/* single internal global stack *
static LogStack mainStack = {
    .logfile = NULL,
    .supressLog = true,   /* default: file logging disabled until enabled *
    .supressOut = false,
    .supressErr = false,
    .verbosity = VERBOSE, /* default allow everything; user can change *
    .fatal = false,
    .head = NULL,
    .tail = NULL,
    .cleanup_cb = NULL
};

/* constant strings for printing *
static const char* const LS_verbose  = "VERBOSE";
static const char* const LS_info     = "INFO";
static const char* const LS_warning  = "WARNING";
static const char* const LS_critical = "CRITICAL";
static const char* const LS_fatal    = "FATAL";

static const char* const TS_none     = "";
static const char* const TS_system   = "SYSTEM";

/* ---------------------------
   Internal helpers
   --------------------------- *

/* safe strdup implementation *
static char* x_strdup(const char* s)
{
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* p = (char*)malloc(n);
    if (!p) return NULL;
    memcpy(p, s, n);
    return p;
}

static const char* level_to_str(ErrorLevel level)
{
    switch (level)
    {
        case VERBOSE:  return LS_verbose;
        case INFO:     return LS_info;
        case WARNING:  return LS_warning;
        case CRITICAL: return LS_critical;
        case FATAL:    return LS_fatal;
        default:       return "UNKNOWN";
    }
}

static const char* type_to_str(ErrorType type)
{
    switch (type)
    {
        case NONE:   return TS_none;
        case SYSTEM: return TS_system;
        default:     return "UNKNOWN";
    }
}

/* allocate and push a log entry at tail *
static void push_entry_internal(const char* msg, ErrorLevel lvl, ErrorType type)
{
    if (!msg) msg = "(null)";

    /* filter by verbosity *
    if (lvl < mainStack.verbosity) {
        return;
    }

    LogEntry* e = (LogEntry*)malloc(sizeof(LogEntry));
    if (!e) {
        /* Allocation failure: write to stderr and treat as fatal *
        if (!mainStack.supressErr) {
            fprintf(stderr, "[%s] %s\n", LS_fatal, "Malloc failed while logging; aborting.");
        }
        /* attempt to flush what we can and exit *
        mainStack.fatal = true;
        if (mainStack.cleanup_cb) mainStack.cleanup_cb();
        /* attempt flush */
        /* avoid recursion by direct minimal writes *
        if (mainStack.logfile) fflush(mainStack.logfile);
        exit(EXIT_FAILURE);
    }

    e->msg = x_strdup(msg);
    if (!e->msg) {
        free(e);
        if (!mainStack.supressErr) {
            fprintf(stderr, "[%s] %s\n", LS_fatal, "Malloc failed while duplicating log message; aborting.");
        }
        mainStack.fatal = true;
        if (mainStack.cleanup_cb) mainStack.cleanup_cb();
        if (mainStack.logfile) fflush(mainStack.logfile);
        exit(EXIT_FAILURE);
    }

    e->level = lvl;
    e->type = type;
    e->timestamp = time(NULL);
    e->next = NULL;

    if (!mainStack.head) {
        mainStack.head = mainStack.tail = e;
    } else {
        mainStack.tail->next = e;
        mainStack.tail = e;
    }
}

/* format time to human-readable string (localtime) *
static void format_timestamp(time_t t, char* buf, size_t bufsz)
{
    if (!buf || bufsz == 0) return;
    struct tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    strftime(buf, bufsz, "%Y-%m-%d %H:%M:%S", &tm);
}

/* write a single entry to outputs (file/stdout/stderr depending on level) *
static void write_entry(const LogEntry* e)
{
    if (!e) return;

    char timestr[32];
    format_timestamp(e->timestamp, timestr, sizeof(timestr));

    /* formatted line */
    /* Example: [2025-11-30 12:34:56] [ERROR] [SYSTEM] message text\n *
    const char* level_s = level_to_str(e->level);
    const char* type_s = type_to_str(e->type);

    /* file output *
    if (!mainStack.supressLog && mainStack.logfile) {
        if (fputs("[", mainStack.logfile) >= 0) {
            fprintf(mainStack.logfile, "%s] [%s] [%s] %s\n", timestr, level_s, type_s, e->msg ? e->msg : "");
            fflush(mainStack.logfile);
        }
    }

    /* stdout for INFO and VERBOSE *
    if (!mainStack.supressOut && (e->level == INFO || e->level == VERBOSE)) {
        printf("[%s] [%s] [%s] %s\n", timestr, level_s, type_s, e->msg ? e->msg : "");
    }

    /* stderr for WARNING/CRITICAL/FATAL *
    if (!mainStack.supressErr && (e->level == WARNING || e->level == CRITICAL || e->level == FATAL)) {
        fprintf(stderr, "[%s] [%s] [%s] %s\n", timestr, level_s, type_s, e->msg ? e->msg : "");
    }
}

/* flush all buffered entries to configured outputs and free them *
void log_flush(void)
{
    LogEntry* cur = mainStack.head;
    while (cur) {
        write_entry(cur);
        LogEntry* next = cur->next;
        if (cur->msg) free(cur->msg);
        free(cur);
        cur = next;
    }
    mainStack.head = mainStack.tail = NULL;
}

/* shutdown without exit: flush and close file if open *
void log_shutdown(void)
{
    log_flush();
    if (mainStack.logfile) {
        fclose(mainStack.logfile);
        mainStack.logfile = NULL;
    }
    mainStack.supressLog = true;
}

/* handle fatal error: flush, call cleanup callback, close file, and exit *
static void handle_fatal(void)
{
    mainStack.fatal = true;
    /* flush entries (including the fatal entry itself) *
    log_flush();

    /* call cleanup cb if available *
    if (mainStack.cleanup_cb) {
        mainStack.cleanup_cb();
    }

    /* close logfile *
    if (mainStack.logfile) {
        fclose(mainStack.logfile);
        mainStack.logfile = NULL;
    }

    /* exit the program with failure code *
    exit(EXIT_FAILURE);
}

/* ---------------------------
   Public API implementation
   --------------------------- *

/* Primary 4 functions *

void log_error(const char* msg, ErrorLevel lvl, ErrorType type)
{
    /* ensure not already exiting due to fatal *
    if (mainStack.fatal) {
        /* if program is already shutting down, do nothing *
        return;
    }

    /* default type if invalid *
    if (type != SYSTEM && type != NONE) {
        type = NONE;
    }

    /* push the entry into the buffer (will be discarded if below verbosity) *
    push_entry_internal(msg, lvl, type);

    /* if it's fatal, ensure immediate handling *
    if (lvl == FATAL) {
        handle_fatal();
    }
}

void log_msg(const char* msg)
{
    log_error(msg, INFO, NONE);
}

void log_lvl(const char* msg, ErrorLevel lvl)
{
    log_error(msg, lvl, NONE);
}

void log_type(const char* msg, ErrorType type)
{
    log_error(msg, INFO, type);
}

/* ---------------------------
   Configuration helpers
   --------------------------- *

void set_log_verbosity(ErrorLevel lvl)
{
    mainStack.verbosity = lvl;
}

ErrorLevel get_log_verbosity(void)
{
    return mainStack.verbosity;
}

/* enable file logging; returns true on success *
bool log_enable_file(const char* path)
{
    if (!path) return false;
    FILE* f = fopen(path, "a");
    if (!f) {
        return false;
    }
    /* close any existing *
    if (mainStack.logfile) fclose(mainStack.logfile);
    mainStack.logfile = f;
    mainStack.supressLog = false;
    return true;
}

void log_disable_file(void)
{
    if (mainStack.logfile) {
        fclose(mainStack.logfile);
        mainStack.logfile = NULL;
    }
    mainStack.supressLog = true;
}

void log_set_stdout_enabled(bool enabled)
{
    mainStack.supressOut = !enabled;
}

void log_set_stderr_enabled(bool enabled)
{
    mainStack.supressErr = !enabled;
}

void register_cleanup(void (*cleanup_cb)(void))
{
    mainStack.cleanup_cb = cleanup_cb;
}

// TODO: adding a cleanup job returns an ID, which can be used to then mark that cleanup job as void

*/