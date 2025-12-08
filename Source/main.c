#include "util/util.h"

#include "load/load.h"
#include "read/read.h"
#include "process/process.h"
#include "execute/execute.h"

#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[])
{
    // register before parsing; no need to clear this job
    register_cleanup(&clear_config);
    const Config* settings = parse_settings(argc, (const char* const*)argv);

    // Actionnable settings
    if(settings->verbose) set_verbosity(VERBOSE);
    if(settings->help) print_help();

    // Step 1: Load
    const Cache *cache = NULL;
    if(!settings->atomic)
        cache = load_cache();
    register_cleanup(&close_cache);
    log_status(settings->statuses);

    if(settings->clear)
        clear_cache();

    // Step 2: Read

    // Step 3: Process

    // Step 4: Execute
    const char* groupString = get_host_group_name();
    printf("Hosting group is: %s\n", groupString);

    clear_config(settings);
    close_logging();
    return 0;
}