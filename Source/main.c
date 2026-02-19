#include "util/util.h"

// #include "load/load.h"
// #include "read/read.h"
// #include "process/process.h"
#include "execute/execute.h"

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
    register_cleanup(clear_config);
    const Config* settings = parse_settings(argc, (const char* const*)argv);

    // Step 0: Prepare
    if(settings->verbose) set_verbosity(VERBOSE);
    if(settings->help) print_help();
    if(settings->statuses) printf("Statuses to print: %s\n", settings->statuses);
    const char* pipeline = DEFAULT_PIPELINE;
    if(settings->inputFile) pipeline = settings->inputFile;

    // Step 1: Load

    // Step 2: Read

    // Step 3: Process

    // Step 4: Execute
    init_workers(settings->jobs);
    register_cleanup(close_workers);
    ShellCommand newCommand = (ShellCommand){"cd build", "./", 0};
    runCommand(newCommand);
    // runPipe();


    const char* groupString = get_host_group_name();
    log_msg("Hosting group is: %s", groupString);

    close_workers();
    clear_config(settings);
    close_logging();
    return 0;
}

/*
 * Plan of execution:
 * - Build minimal executor: pass direct pipeline commands to execution
 * - Build minimal reader: elementary processing of pipeline file -> minimal cachable state
 * - 
 * 
 * 
 * The full pipeline execution model is as follows:
 * - 1. Load: Load the cache (if any) and use it to configure default pipe behavior
 * - 2. Read: Read the pipeline file and determine the extent of it's validity (has it been renewed?)
 *            Also includes the interpretation and caching of the file if it is new, to create a cachable state.
 * - 3. Process: Process the cache data in order to determine the build graph and it's execution pipeline.
 * - 4. Execute: Execute the appropriate commands in the appropriate order, determined by the processing state.
 * 
 * need to define structs for actions, pipes and flows.
 * executor takes in a string-like command and simply, naively executes it, prints result, fails, checks and makes sure execution is a-ok.
 * Process is the one that takes an action and uses pipe struct to determine variables mappings and transforms them into string-commands.
 * Read is the one that parses and creates action, pipe and flow structs.
 */