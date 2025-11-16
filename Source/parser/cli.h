#pragma once

#include "../global.h"


typedef 
enum OptionType
{
    ERROR = -1,
    END,    // ending block

    FLOW,   // default no arg

    HELP,   // -h, --help
    CONFIG, // -c, --config
    CLEAR,  // --clear
    ATOMIC, // -a, --atomic
    VERBOSE,// -v, --verbose
    STATUS, // -s, --status <state>
    PARSE,  // -p, --parse [s|e]
    DEFINE, // -d, --define <var>[=<value>]
    JOBS,   // -j, --jobs <N> 
    INPUT   // -f, --file <input_file>      ( FILE is already used )
}OptionType;



typedef 
struct Parameter
{
    OptionType type;
    const char *argument;
}Parameter;


typedef
struct ConfigError
{
    const char* msg;
    struct ConfigError *next;
} ConfigError;


typedef 
struct Config{
    ConfigError *errors;   // parse errors ( TODO: replace char -> parseError )
    const char **flows; // list of <const char[]> to run
    size_t flow_count;
    bool help;
    bool doConfig;
    char *statuses; //single character array
    bool clear;
    bool atomic;
    bool verbose;
    char parse; // d: default, s: static, e: emit
    const char **defines; // array of "key=value" strings
    size_t define_count;
    unsigned int jobs;
    const char *inputFile;
}Config;


Config parseSettings(int argc, const char* const argv[]);
void clearConfig(Config config);

void printHelp();