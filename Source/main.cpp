#include <iostream>
#include <vector>

#include "parser/parser.hpp"




int main(int argc, char* argv[])
{
    std::vector<std::string> arguments(argc-1);
    for(int i = 1; i<argc; i++)
        arguments[i-1] = std::string(argv[i]);


    std::vector<Option> options;
    if(--argc>=1)
    {
        options = parseOptions(argc, arguments);
    }

    for (auto arg : options)
    {
        switch (arg.option)
        {
        case ERROR: std::cout << "ERROR"; break;
        case NONE: std::cout << "NONE"; break;
        case FLOW: std::cout << "FLOW"; break;
        case HELP: std::cout << "HELP"; break;
        case CONFIG: std::cout << "CONFIG"; break;
        case STATUS: std::cout << "STATUS"; break;
        case CLEAR: std::cout << "CLEAR"; break;
        case ATOMIC: std::cout << "ATOMIC"; break;
        case VERBOSE: std::cout << "VERBOSE"; break;
        case PARSE: std::cout << "PARSE"; break;
        case DEFINE: std::cout << "DEFINE"; break;
        case JOBS: std::cout << "JOBS"; break;
        case INPUT: std::cout << "INPUT"; break;
        default:
            std::cout << "INDETERMNINATE: ";
        }

        if(!arg.value.empty())
            std::cout << ": " << arg.value;
        std::cout << std::endl;
    }
    if(options.size() == 0) std::cout<<"No options to print" << std::endl;
    

    // printHelp();
    return 0;
}