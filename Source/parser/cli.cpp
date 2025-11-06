#include "cli.hpp"

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
    "   -c, --config                    : Reconfigure the pipe file." << std::endl <<
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
    "   -d, --define <variable>         : Define a variable to be used during execution." << std::endl <<
    "                                     Defined variable has the same priority as a CONFIG," << std::endl <<
    "                                     but takes priority ove config-stage variables." << std::endl <<
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

std::vector<Option> parseOptions(int &argCount, const std::vector<std::string> &arguments)
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
        switch(arguments[arg][optStart])
        {
            case 'h': argOption = HELP; break;
            case 's': argOption = STATUS; break;
            case 'a': argOption = ATOMIC; break;
            case 'v': argOption = VERBOSE; break;
            case 'p': argOption = PARSE; break;
            case 'd': argOption = DEFINE; break;
            case 'j': argOption = JOBS; break;
            case 'f': argOption = INPUT; break;
            case 'c':   // can be either CONFIG or CLEAR
                argOption = CONFIG;
                if(arguments[arg][optStart+1] == 'l')
                    argOption = CLEAR; break;
                break;
            default: argOption = ERROR; break;
        }

        std::string argValue = "";
        if(optStart == 1 && arguments[arg][optStart+1])
            argValue = std::string(&arguments[arg][optStart+1]);
        else if(arg+1 < argCount && arguments[arg+1][0] != '-') // no segfault cause lazy evaluation
            argValue = std::string(arguments[++arg]); optionCount--;

        options.push_back(Option{argOption, argValue});

        // if(optStart >= 2 && arguments[arg][optStart] == 'c') optStart++; // in that case, option is either
    }

    argCount = optionCount;
    return options;
}