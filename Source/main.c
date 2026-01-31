#include "util/util.h"

// #include "load/load.h"
// #include "read/read.h"
// #include "process/process.h"
// #include "execute/execute.h"

#include <stdio.h>
#include <stdlib.h>


void runPipe()
{
    printf("RUNNING PIPE\n");
    return;
}



int main(int argc, char* argv[])
{
    // register before parsing; no need to clear this job
    register_cleanup(&clear_config);
    const Config* settings = parse_settings(argc, (const char* const*)argv);

    // Actionnable settings
    if(settings->verbose) set_verbosity(VERBOSE);
    if(settings->help) print_help();
    if(settings->statuses) printf("Statuses to print: %s\n", settings->statuses);
    const char* pipeline = DEFAULT_PIPELINE;
    if(settings->inputFile) pipeline = settings->inputFile;

    // Step 1: Load
    // Step 2: Read
    // Step 3: Process
    // Step 4: Execute
    runPipe();


    const char* groupString = get_host_group_name();
    printf("Hosting group is: %s\n", groupString);

    clear_config(settings);
    close_logging();
    return 0;
}