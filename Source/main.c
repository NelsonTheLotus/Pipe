#include "util/util.h"

#include "load/load.h"
#include "read/read.h"
#include "process/process.h"
#include "execute/execute.h"

#include <stdio.h>
#include <stdlib.h>

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
    // register before parsing; no need to clear this job
    register_cleanup(&clear_config);
    Config* settings = parse_settings(argc, (const char* const*)argv);

    // Actionnable settings
    if(settings->verbose) set_verbosity(VERBOSE);
    if(settings->help) print_help();

    // Step 1: Load
    const Cache *cache = NULL;
    if(!settings->atomic)
        cache = load_cache();
    register_cleanup(&close_cache);

    if(settings->statuses != NULL)
        printStatuses(settings->statuses);
    else if(settings->clear)
        clearCache();

    // Step 2: Read

    // Step 3: Process

    // Step 4: Execute
    const char* groupString = get_host_group_name();
    printf("Hosting group is: %s\n", groupString);

    clear_config(settings);
    close_logging();
    return 0;
}