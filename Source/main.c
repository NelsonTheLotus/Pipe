#include "parser/parser.h"

#include <stdio.h>



int main(int argc, char* argv[])
{
    Config settings = parseSettings(argc, (const char* const*)argv);

    // int counter = 0;
    // int *pc = &counter;
    // while(*pc <= 10)
    // {
    //     (*pc)++;
    //     printf("POINTER: %p | ", pc);
    //     printf("ITERATION: %d\n", counter);
    // }

    // printf("NONE: %p\n", settings.errors);
    // printf("FLOW: %s\n", settings.flows[0]);
    // printf("HELP: %p\n", settings[HELP]);
    // printf("CONFIG: %p\n", settings[CONFIG]);
    // printf("STATUS: %p\n", settings[STATUS]);
    // printf("CLEAR: %p\n", settings[CLEAR]);
    // printf("ATOMIC: %p\n", settings[ATOMIC]);
    // printf("VERBOSE: %p\n", settings[VERBOSE]);
    // printf("PARSE: %p\n", settings[PARSE]);
    // printf("DEFINE: %s\n", settings.defines[0]);
    // printf("JOBS: %u\n", settings.jobs);
    // printf("INPUT: %p\n", settings[INPUT]);

    clearConfig(settings);
    return 0;
}