#pragma once

#include "../global.h"


typedef 
struct Config{
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


Config* parse_settings(int argc, const char* const argv[]);
void clear_config();

void print_help();