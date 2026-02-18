#include "terminal.h"

#include "log.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>



// ==== Internal structs ====

typedef enum OptionType
{
    C_ERROR = -1,
    C_END,    // ending block

    C_FLOW,   // default no arg

    C_HELP,   // -h, --help
    C_CONFIG, // -c, --config
    C_CLEAR,  // --clear
    C_ATOMIC, // -a, --atomic
    C_VERBOSE,// -v, --verbose
    C_STATUS, // -s, --status <state>
    C_PARSE,  // -p, --parse [s|e]
    C_DEFINE, // -d, --define <var>[=<value>]
    C_JOBS,   // -j, --jobs <N> 
    C_INPUT   // -f, --file <input_file>      ( FILE is already used )
}OptionType;


typedef struct Parameter
{
    OptionType type;
    const char *argument;
}Parameter;



// ==== Static configuration ====

static Config static_config = (Config)
{
    .flows        = NULL,
    .flow_count   = 0,
    .help         = false,
    .doConfig     = false,
    .statuses     = NULL,
    .clear        = false,
    .atomic       = false,
    .verbose      = false,
    .parse        = 'd',
    .defines      = NULL,
    .define_count = 0,
    .jobs         = 0,
    .inputFile    = NULL    // NULL -> gets interpreted as DEFAULT_INPUT 
};



// ==== Internal Helpers ====

/*
 * Determines the kind of option declared, 
 * taking into account single or double tacks options.
 * Return error if no match found.
 * Option must point to the first char of the option, no a tack
*/
OptionType get_option_type(const char* option, bool isDoubleTack)
{
    switch (option[0])
    {
    case 'h': return C_HELP;
    case 'c':
        if(!isDoubleTack) return C_CONFIG; // --clear is only DoubleTack 
        if(option[1] == '\0') return C_ERROR;
        if(option[1] == 'o') return C_CONFIG; // second letter is 'o' => config option
        return C_CLEAR;                       // second letter is NOT 'o' (can check for 'l', but not necessary) => clear
    case 's': return C_STATUS;
    case 'a': return C_ATOMIC;
    case 'v': return C_VERBOSE;
    case 'p': return C_PARSE;
    case 'd': return C_DEFINE;
    case 'j': return C_JOBS;
    case 'f': return C_INPUT;
    
    default:
        return C_ERROR;
    }

    return C_END;     // failsafe
}



// ==== Interface ====

/*
 * Fetches the next parameter from the list, starting at index.
 * Returns a parameter with the parameter type and it's argument, if any.
 * Increments the index variable so that it can be used in the next iteration.
*/
Parameter get_next_parameter(const int argc, int *index, const char* const argv[])
{
    if(index == NULL) index = (int*)&index; // make valid ptr pointing to null. Works because size(ptr) >= size(int)
    if(*index == 0) (*index)++; // ignore program call
    if(*index >= argc) return (Parameter){C_END, NULL}; // no args to parse -> END reached

    const char *option = argv[(*index)++];
    if(strlen(option) <= 1) return (Parameter){C_FLOW, option}; // cannot be an option, cause option is at least 2.
    if(option[0] != '-') return (Parameter){C_FLOW, option};  // no tacks => flow

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
    OptionType paramType = get_option_type(option, isDoubleTack);
    if(paramType <= C_VERBOSE) return (Parameter){paramType, argValue};

    // search for arg value at next option if not already assigned
    if(*index < argc && argValue == NULL && argv[*index][0] != '-')
        argValue = argv[(*index)++];

    return (Parameter){paramType, argValue};
}


/*
 * Iterates through CLI options and classifies them into their 
 * corresponding setting in the Config structure. 
*/
const Config* parse_settings(int argc, const char* const argv[])
{
    int counter = 0;

    bool endReached = false;
    const char ***list_ptr = NULL;
    size_t *list_count = NULL;
    while(!endReached)
    {
        Parameter nextParam = get_next_parameter(argc, &counter, argv);
        switch (nextParam.type)
        {
        case C_HELP: static_config.help = true; break;
        case C_CONFIG: static_config.doConfig = true; break;
        case C_CLEAR: static_config.clear = true; break;
        case C_ATOMIC: static_config.atomic = true; break;
        case C_VERBOSE: static_config.verbose = true; break;
        case C_PARSE: static_config.parse = nextParam.argument[0]; break;
        case C_JOBS: static_config.jobs = (unsigned int)strtoul(nextParam.argument, NULL, 10); break;
        case C_INPUT: static_config.inputFile = nextParam.argument; break;

        case C_FLOW:
            list_ptr = &(static_config.flows);
            list_count = &(static_config.flow_count);
            goto extend_list;

        case C_DEFINE:
            list_ptr = &(static_config.defines);
            list_count = &(static_config.define_count);
        extend_list:
            const char **tmp_list = realloc(*list_ptr, ++(*list_count) * sizeof(*tmp_list));
            if(!tmp_list) goto error;
            tmp_list[(*list_count)-1] = nextParam.argument;
            *list_ptr = tmp_list;
            break;

        case C_STATUS:
            size_t statlen = 0;
            if(static_config.statuses != NULL) statlen = strlen(static_config.statuses); // count null char
            char *tmp_char = realloc(static_config.statuses, statlen+2); // add a charachter + null char
            if(!tmp_char) goto error;
            tmp_char[statlen+1] = '\0';
            tmp_char[statlen] = nextParam.argument[0];
            static_config.statuses = tmp_char;
            break;

        case C_END:
            endReached = true;
            break;

        error:
        case C_ERROR: // same as default
        default:
            log_fatal(CLI, "Could not allocate required space to parse CLI options.");
            break;
        }
    }

    return &static_config;
}


/*
 * Free space that was allocated on the heap to store config.
*/
void clear_config()
{
    if(static_config.flows != NULL)
        free(static_config.flows);
    if(static_config.statuses != NULL)
        free(static_config.statuses);
    if(static_config.defines != NULL)
        free(static_config.defines);

    // safe double clear
    static_config.flows = NULL;
    static_config.statuses = NULL;
    static_config.defines = NULL;
}



/*
 * Print help text
*/
void print_help()
{
    
    printf("Usage: pipe [<flow>] [<options>] [-f <pipe_file>]\n\n");

    printf("By default, if no <flow> is not specified, Pipe will run the flow\n");
    printf("marked as default, or the first occuring flow if none defined.\n\n");

    printf("If no <pipe_file> is specified, Pipe searches the cache for\n"); 
    printf("the previously used pipe file. If not found, it then searches\n");
    printf("the CWD for a file named 'pipefile'. If not found, it fails.\n\n");

    printf("Options include:\n");
    printf("   -h, --help                      : Show this text.\n");
    printf("   -c, --config                    : Reconfigure the pipe file. Config is automatically\n");
    printf("                                     run if pipe file is newer than cache.\n");
    printf("   -s, --status [<state>]          : Displays pipes cache state. \n");
    printf("                                     State may be 'config', ....\n");
    printf("                                     If no state is specified, all states will be shown.\n");
    printf("   --clear                         : Clear all cache and data.\n");
    printf("   -a, --atomic                    : Run pipe atomically, ignoring all cache states.\n");
    printf("   -v, --verbose                   : Enable verbose logging.\n");
    printf("   -p, --parse [s|e]               : Parse and validate pipe file.\n");
    printf("                                     If run with 's', do static analysis only.\n");
    printf("                                     If run with 'e', emit generated artifacts to cache.\n");
    printf("                                     If validation fails, cache is not altered.\n");
    printf("                                     If no parameter is declared, defaults to e.\n");
    printf("   -d, --define <var>[=<value>]    : Define a variable to be used during execution.\n");
    printf("                                     Defined variable has the same priority as a CONFIG,\n");
    printf("                                     but takes priority over config-stage variables.\n");
    printf("   -j. --jobs <N>                  : Run pipe using at most N jobs. If <N> is omitted,\n");
    printf("                                     use as many jbos as necessary. Default is 1.\n");
    printf("   -f. --file <pipe_file>          : Specifies the input Pipe file.\n\n");

    printf("When declaring option parameters, if the option is declared using it's single charachter form,\n");
    printf("the parameter may be declared with no whitespace seperation. For example, the following\n");
    printf("options are valid and produce an identical result:\n");
    printf("   -sconfig\n");
    printf("   --status config\n");
    printf("   -s config\n\n");

    printf("However, the following is not allowed:\n");
    printf("   --statusconfig\n\n");

    return;
}
