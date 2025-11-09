#include "cli.hpp"

#include "../global.hpp"

#include <iostream>
#include <vector>


void printHelp()
{
    std::cout <<
    "Usage: pipe [<flow>] [<options>] [-f <pipe_file>]"
    <<std::endl<<std::endl<<
    "By default, if <flow> is not specified, Pipe will run the flow" << std::endl <<
    "marked as default, or the first occuring flow. " << std::endl <<
    std::endl <<
    "If no <pipe_file> is specified, Pipe searches the cache for" << std::endl << 
    "the previously used pipe file. If not found, it then searches"  << std::endl <<
    "the CWD for a file named 'pipefile'. If not found found, it fails."
    << std::endl << std::endl<<
    "Options include:" << std::endl <<
    "   -h, --help                      : Show this text." << std::endl <<
    "   -c, --config                    : Reconfigure the pipe file. Config is automatically" << std::endl <<
    "                                     run if pipe file is newer than cache." << std::endl <<
    "   -s, --status [<state>]          : Displays pipes cache state. "  << std::endl <<
    "                                     State may be 'config', ...."  << std::endl <<
    "                                     If no state is specified, all states will be shown." << std::endl <<
    "   --clear                         : Clear all cache and data." << std::endl <<
    "   -a, --atomic                    : Run pipe, ignoring all cache states." << std::endl <<
    "   -v, --verbose                   : Enable verbose logging." << std::endl <<
    "   -p, --parse [s|e]               : Parse and validate pipe file."  << std::endl <<
    "                                     If run with 's', do static analysis only."  << std::endl <<
    "                                     If run with 'e', emit generated artifacts to cache."  << std::endl <<
    "                                     If validation fails, cache is not altered." << std::endl <<
    "                                     If no parameter is declared, defaults to e." << std::endl <<
    "   -d, --define <var>[=<value>]    : Define a variable to be used during execution." << std::endl <<
    "                                     Defined variable has the same priority as a CONFIG," << std::endl <<
    "                                     but takes priority over config-stage variables." << std::endl <<
    "   -j. --jobs <N>                  : Run pipe using at most N jobs. If <N> is omitted," << std::endl <<
    "                                     use as many jbos as necessary. Default is 1." << std::endl <<
    "   -f. --file <pipe_file>          : Specifies the input Pipe file."
    << std::endl << std::endl <<
    "When declaring option parameters, if the option is declared using it's single charachter form,"  << std::endl <<
    "the parameter may be declared with no whitespace seperation. For example, the following"  << std::endl <<
    "options are valid and produce an identical result:"  << std::endl <<
    "   -sconfig"  << std::endl <<
    "   --status config"  << std::endl <<
    "   -s config"  << std::endl << std::endl <<
    "However, the following is not allowed:"  << std::endl <<
    "   --statusconfig" << std::endl <<
    std::endl;

    return;
}


/*
 * Destroys all allocated space for settings
*/
void clearSettings(std::array<void*, 12> settings)
{
    // NONE DEFINE STATUS FLOW
    for(auto option: {NONE, DEFINE, STATUS, FLOW})
    {
        Setting* current_ptr = nullptr;
        Setting* next_ptr = (Setting*)settings[option];

        while(next_ptr != nullptr)
        {
            current_ptr = next_ptr;
            next_ptr = current_ptr->previousSetting;
            delete current_ptr;
        }
    }

    if(settings[INPUT] != nullptr)
        delete settings[INPUT];

    return;
}



/*
 * Parses CLI options and puts them into a vector of Options.
 * A dot `.` may be used as a replacement for an argument value.
 * In that case, option will be interpreted to have no assigned values.
 * This allows an option line like so "-s flow" not to be interpreted
 * as "--static=flow", but rather as "--static flow=flow" by writting it like
 * so: "-s. flow" or "--static . flow".
 */
std::vector<Option> parseOptions(int &argCount, char* arguments[])
{
    //allocate sufficient space for the case that all args are individual options
    auto optionCount = argCount; 
    std::vector<Option> options;
    options.reserve(argCount);

    for(int arg = 0; arg < argCount; arg++)
    {
        unsigned int optStart = 0;
        while(arguments[arg][optStart] == '-') optStart++;

        if(optStart == 0)
        {
            options.push_back(Option{FLOW, std::string(arguments[arg])});
            continue;
        }

        OptionType argOption = NONE;
        bool parseArg = false;
        switch(arguments[arg][optStart])
        {
            case 'h': argOption = HELP; break;
            case 's': argOption = STATUS; parseArg = true; break;
            case 'a': argOption = ATOMIC; break;
            case 'v': argOption = VERBOSE; break;
            case 'p': argOption = PARSE; parseArg = true; break;
            case 'd': argOption = DEFINE; parseArg = true; break;
            case 'j': argOption = JOBS; parseArg = true; break;
            case 'f': argOption = INPUT; parseArg = true; break;
            case 'c':   // can be either CONFIG or CLEAR
                argOption = CONFIG;
                if(arguments[arg][optStart+1] == 'l')
                    argOption = CLEAR; break;
                break;
            default: argOption = ERROR; break;
        }
        
        std::string argValue = "";
        if(parseArg)
        {
            if(optStart == 1 && arguments[arg][optStart+1])
                argValue = std::string(&arguments[arg][optStart+1]);
            else if(arg+1 < argCount && arguments[arg+1][0] != '-') // no segfault cause lazy evaluation
                argValue = std::string(arguments[++arg]); optionCount--;
        }
        options.push_back(Option{argOption, argValue});

        // if(optStart >= 2 && arguments[arg][optStart] == 'c') optStart++; // in that case, option is either
    }

    argCount = optionCount;
    return options;
}


/*
 * Parses retirved options and sets the according values into a
 * void-pointer array of deterministic size.
 * Each pointer position corresponds to the following values:
 * - NONE: list of errors
 * - FLOW: points to a list of flows to run in sequence
 * - HELP: (bool) weather to print help of not
 * - CONFIG: weather to rerun config or not
 * - STATUS: points to a list of statuses to print
 * - CLEAR: weather to clear cache. 
 * - ATOMIC: weather to run this as atomic
 * - VERBOSE: weather to print additionnal information
 * - PARSE: ptr to string qualifying parse type; nil if default
 * - DEFINE: Points to a list of defined variables
 * - JOBS: number of jobs to run; default 1, 0 if no option value
 * - INPUT: Points to a string input file path to use
 */
std::array<void*, 12> getSettings(int &argCount, char* arguments[])
{
    std::vector<Option> options;
    if(argCount > 0)
        options = parseOptions(argCount, arguments);

    std::array<void*, 12> settings = 
    {
        nullptr,        // NONE: no erros
        nullptr,        // FLOW: no flows
        nullptr,        // HELP: no print
        nullptr,        // CONFIG: no run
        nullptr,        // STATUS: no print
        nullptr,        // CLEAR: no clear
        nullptr,        // ATOMIC: use cache
        nullptr,        // VERBOSE: no verbose
        nullptr,        // PARSE: no parse-only
        nullptr,        // DEFINE: no predefined vars
        (void*)1,       // JOBS: default 1 job
        nullptr         // INPUT: use default file if nil
    };

    Setting *latestSetting = new Setting;
    for(size_t argID = 0; argID < options.size(); argID++)
    {
        latestSetting->type = options[argID].option;
        switch (auto opttype = options[argID].option)
        {
        case HELP: settings[HELP] = (void*)1; break;
        case CONFIG: settings[CONFIG] = (void*)1; break;
        case CLEAR: settings[CLEAR] = (void*)1; break;
        case ATOMIC: settings[ATOMIC] = (void*)1; break;
        case VERBOSE: settings[VERBOSE] = (void*)1; break;
        case JOBS: settings[JOBS] = (void*)std::stoi(options[argID].value); break;
        
        case FLOW: // nearly same process as status
        case STATUS: { // same process as flow
            latestSetting->key = "";
            int keyid = -1;
            if(opttype == FLOW)
            {
                if(settings[FLOW] != nullptr)
                    keyid = std::stoi(((Setting*)settings[FLOW])->key);
                latestSetting->key = std::to_string(keyid+1);
            }
            latestSetting->value = options[argID].value;
            latestSetting->previousSetting = (Setting*)settings[opttype];
            settings[opttype] = latestSetting;
            latestSetting = new Setting;
            break;
        }
        
        case INPUT: 
            if(settings[INPUT] != nullptr) delete settings[INPUT];
            settings[INPUT] = new std::string(options[argID].value);
            break;

        case PARSE:
            settings[PARSE] = (void*)options[argID].value[0];
            break;

        case DEFINE: { 
            size_t equality = options[argID].value.find_first_of('=', 0);
            if(equality == std::string::npos) equality = options[argID].value.size();
            latestSetting->key = std::string(options[argID].value, equality);
            if(options[argID].value[equality] != 0)
                latestSetting->value = std::string(&options[argID].value[equality+1], options[argID].value.size()-equality);

            latestSetting->previousSetting = (Setting*)settings[DEFINE];
            settings[DEFINE] = latestSetting;
            latestSetting = new Setting;
            break;
        }
            
        case ERROR: // treat the same as none
        case NONE:  // same as default
        default:
            latestSetting->key = options[argID].value;
            latestSetting->previousSetting = (Setting*)settings[NONE];
            settings[NONE] = latestSetting;
            latestSetting = new Setting;
            break;
        }
    }

    delete latestSetting;
    return settings;
}


