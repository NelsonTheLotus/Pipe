#pragma once

#include <string>
#include <vector>

enum OptionType : int
{
    ERROR = -1,
    NONE,

    FLOW,   // default no arg

    HELP,   // -h, --help
    CONFIG, // -c, --config
    STATUS, // -s, --status
    CLEAR,  // --clear
    ATOMIC, // -a, --atomic
    VERBOSE,// -v, --verbose
    PARSE,  // -p, --parse [s|e]
    DEFINE, // -d, --define <var> <value>
    JOBS,   // -j, --jobs <N> 
    INPUT   // -f, --file       ( FILE is already used )
};

struct Option
{
    OptionType option;
    std::string value;
};

void printHelp();
std::vector<Option> parseOptions(int &argCount, const std::vector<std::string> &arguments);