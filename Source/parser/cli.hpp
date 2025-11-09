#pragma once

#include <string>
#include <vector>
#include <array>

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

struct Setting
{
    OptionType type;
    std::string key;
    std::string value;
    struct Setting* previousSetting;
};

void printHelp();
void clearSettings(std::array<void*, 12> settings);
std::array<void*, 12> getSettings(int &argCount, char* arguments[]);