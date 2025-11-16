#include "cli.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*
 * Determines the kind of option declared, 
 * taking into account single or double tacks options.
 * Return error if no match found.
 * Option must point to the first char of the option, no a tack
*/
OptionType getOptionType(const char* option, bool isDoubleTack)
{
    switch (option[0])
    {
    case 'h': return HELP;
    case 'c':
        if(!isDoubleTack) return CONFIG; // --clear is only DoubleTack 
        if(option[1] == '\0') return ERROR;
        if(option[1] == 'o') return CONFIG; // second letter is 'o' => config option
        return CLEAR;                       // second letter is NOT 'o' (can check for 'l', but not necessary) => clear
    case 's': return STATUS;
    case 'a': return ATOMIC;
    case 'v': return VERBOSE;
    case 'p': return PARSE;
    case 'd': return DEFINE;
    case 'j': return JOBS;
    case 'f': return INPUT;
    
    default:
        return ERROR;
    }

    return END;     // failsafe
}


/*
 * Fetches the next parameter from the list, starting at index.
 * Returns a parameter with the parameter type and it's argument, if any.
 * Increments the index variable so that it can be used in the next iteration.
*/
Parameter getNextParameter(const int argc, int *index, const char* const argv[])
{
    if(index == NULL) index = (int*)&index; // make valid ptr pointing to null. Works because size(ptr) >= size(int)
    if(*index == 0) (*index)++; // ignore program call
    if(*index >= argc) return (Parameter){END, NULL}; // no args  to parse

    const char *option = argv[(*index)++];
    if(strlen(option) <= 1) return (Parameter){FLOW, option}; // cannot be an option, cause option is at least 2.
    if(option[0] != '-') return (Parameter){FLOW, option};  // no tacks => flow

    bool isDoubleTack = true;
    const char* argValue = NULL;
    if(option[1] != '-') // else: double tack
    {
        isDoubleTack = false;
        if(strlen(option) >= 3) argValue = &option[2];
    }
    else option = &option[1]; // else: double-tack option
    option = &option[1];

    // determine option and part-value
    OptionType paramType = getOptionType(option, isDoubleTack);
    if(paramType <= VERBOSE) return (Parameter){paramType, argValue};

    // search for arg value at next option if not already assigned
    if(*index < argc && argValue == NULL && argv[*index][0] != '-')
        argValue = argv[(*index)++];

    return (Parameter){paramType, argValue};
}


/*
 * Iterates through CLI options and classifies them into their 
 * corresponding setting in the Config structure. 
*/
Config parseSettings(int argc, const char* const argv[])
{
    int counter = 0;
    Config retConf = (Config)
    {
        NULL,   // errors
        NULL,   // flows
        0,      // flow_count
        false,  // help
        false,  // doConfig
        NULL,   // statuses
        false,  // clear
        false,  // atomic
        false,  // verbose
        'd',    // parse
        NULL,   // defines
        0,      // define_count
        0,      // jobs
        NULL    // input file, gets interpreted as DEFAULT_INPUT if null
    };

    bool endReached = false;
    const char ***list_ptr = NULL;
    size_t *list_count = NULL;
    while(!endReached)
    {
        Parameter nextParam = getNextParameter(argc, &counter, argv);
        switch (nextParam.type)
        {
        case HELP: retConf.help = true; break;
        case CONFIG: retConf.doConfig = true; break;
        case CLEAR: retConf.clear = true; break;
        case ATOMIC: retConf.atomic = true; break;
        case VERBOSE: retConf.verbose = true; break;
        case PARSE: retConf.parse = nextParam.argument[0]; break;
        case JOBS: retConf.jobs = (unsigned int)strtoul(nextParam.argument, NULL, 10); break;
        case INPUT: retConf.inputFile = nextParam.argument; break;
        
        case FLOW:
            list_ptr = &(retConf.flows);
            list_count = &(retConf.flow_count);
            goto extend_list;

        case DEFINE:
            list_ptr = &(retConf.defines);
            list_count = &(retConf.define_count);
        extend_list:
            const char **tmp_list = realloc(*list_ptr, ++(*list_count) * sizeof(*tmp_list));
            if(!tmp_list) goto error;
            tmp_list[(*list_count)-1] = nextParam.argument;
            *list_ptr = tmp_list;
            break;

        case STATUS:
            size_t statlen = 0;
            if(retConf.statuses != NULL) statlen = strlen(retConf.statuses); // count null char
            char *tmp_char = realloc(retConf.statuses, statlen+2); // add a charachter + null char
            if(!tmp_char) goto error;
            tmp_char[statlen+1] = '\0';
            tmp_char[statlen] = nextParam.argument[0];
            retConf.statuses = tmp_char;
            break;

        case END:
            endReached = true;
            break;

        error:
        case ERROR: // same as default
        default:
            printf("ERROR!!\n");
            break;
        }
    }

    return retConf;
}


void clearConfig(Config config)
{
    if(config.errors != NULL) 0;

    if(config.flows != NULL)
        free(config.flows);
    if(config.statuses != NULL)
        free(config.statuses);
    if(config.defines != NULL)
        free(config.defines);
}