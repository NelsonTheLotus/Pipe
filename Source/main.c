#include "util/util.h"
#include "load/load.h"
#include "read/read.h"
#include "process/process.h"
#include "execute/execute.h"

#include <stdio.h>

void printStatuses(const char* statusString)
{
    printf("PRINTING STAUSES: %s\n", statusString);
}

void clearCache()
{
    log_full("CACHE CLEARED", WARNING, SYSTEM);
}

void loadCache()
{
    log_msg("LOADING CACHE..."); // determine weather cache is valid or stale. (kinda)
}



int main(int argc, char* argv[])
{
    Config* settings = parse_settings(argc, (const char* const*)argv);
    register_cleanup(&clear_config); // don't need to clear the job

    bool doSyntax = false;  // Weather to check the file syntax.
    bool doParse = false;   // weather the file must be parsed. Automatic if file is newer than cache.
    bool doConfig = false;  // weather config should be reparsed. auto-set if doParse.
    bool doRun = false;     // weather to actually run the file

    if(settings->help) print_help();

    loadCache(); // framework functions
    if(settings->statuses != NULL)
        printStatuses(settings->statuses);
    else if(settings->clear)
        clearCache();
    else
    {
        //TODO: add logic
        doParse = true; 
        doConfig = true;
        doRun = true;
    }

    // if parseSpec: parse, NOConf

    if(doParse)
        log_t("PARSING...", SYSTEM);
    if(doConfig)
        log_msg("CONFIGURING...");
    if(doRun)
        log_full("RUNNING...", WARNING, SYSTEM);

    //register_cleanup(&clearCache);
    size_t clearCacheID =  register_cleanup(&clearCache);
    register_cleanup(&clearCache);
    remove_cleanup(clearCacheID);

    log_fatal("GENERATED ERROR", NONE);
    

    clear_config(settings);
    close_logging();
    return 0;
}