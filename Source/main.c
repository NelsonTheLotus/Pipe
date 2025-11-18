#include "util/util.h"

#include <stdio.h>

void printStatuses(const char* statusString)
{
    printf("PRINTING STAUSES: %s\n", statusString);
}

void clearCache()
{
    printf("CACHE CLEARED\n");
}

void loadCache()
{
    printf("LOADING CACHE...\n"); // determine weather cache is valid or stale. (kinda)
}



int main(int argc, char* argv[])
{
    Config settings = parseSettings(argc, (const char* const*)argv);

    bool doSyntax = false;  // Weather to check the file syntax.
    bool doParse = false;   // weather the file must be parsed. Automatic if file is newer than cache.
    bool doConfig = false;  // weather config should be reparsed. auto-set if doParse.
    bool doRun = false;     // weather to actually run the file

    if(settings.help) printHelp();

    loadCache();
    if(settings.statuses != NULL)
        printStatuses(settings.statuses);
    else if(settings.clear)
        clearCache();
    else
    {
        //TODO: add logic
        doParse = true; 
        doConfig = true;
        doRun = true;
    }

    // if parseSpec: parse, NOConf

    if(doParse)
        printf("PARSING...\n");
    if(doConfig)
        printf("CONFIGURING...\n");
    if(doRun)
        printf("RUNNING...\n");
    

    clearConfig(settings);
    return 0;
}