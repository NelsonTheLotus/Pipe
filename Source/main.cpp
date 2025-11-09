#include <iostream>
#include <vector>

#include "parser/parser.hpp"




int main(int argc, char* argv[])
{
    // std::vector<std::string> arguments(argc-1);
    // for(int i = 1; i<argc; i++)
    //     arguments[i-1] = std::string(argv[i]);


    std::array<void*, 12> settings = getSettings(--argc, &argv[1]);

    printf("NONE: %p\n", settings[NONE]);
    printf("FLOW: %p\n", settings[FLOW]);
    printf("HELP: %p\n", settings[HELP]);
    printf("CONFIG: %p\n", settings[CONFIG]);
    printf("STATUS: %p\n", settings[STATUS]);
    printf("CLEAR: %p\n", settings[CLEAR]);
    printf("ATOMIC: %p\n", settings[ATOMIC]);
    printf("VERBOSE: %p\n", settings[VERBOSE]);
    printf("PARSE: %p\n", settings[PARSE]);
    printf("DEFINE: %p\n", settings[DEFINE]);
    printf("JOBS: %p\n", settings[JOBS]);
    printf("INPUT: %p\n", settings[INPUT]);
    

    // printHelp();
    clearSettings(settings);
    return 0;
}