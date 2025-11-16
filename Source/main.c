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
    // printf("FLOW: %p\n", settings.flow);
    // printf("HELP: %p\n", settings[HELP]);
    // printf("CONFIG: %p\n", settings[CONFIG]);
    // printf("STATUS: %p\n", settings[STATUS]);
    // printf("CLEAR: %p\n", settings[CLEAR]);
    // printf("ATOMIC: %p\n", settings[ATOMIC]);
    // printf("VERBOSE: %p\n", settings[VERBOSE]);
    // printf("PARSE: %p\n", settings[PARSE]);
    // printf("DEFINE: %p\n", settings[DEFINE]);
    printf("JOBS: %u\n", settings.jobs);
    // printf("INPUT: %p\n", settings[INPUT]);

    return 0;
}