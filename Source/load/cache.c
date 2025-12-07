#include "cache.h"

#include "../global.h"
#include "../util/util.h"

#define CACHE_FOLDER "./.pipe"

#include <stdio.h>
#include <errno.h>


// ==== Internal structures ====


// ==== Static elements ====

static Cache static_cache = (Cache)
{
    false,
};


// ==== Internal Helpers ====

//* Create the directory and files for the cache
static bool create_cache(void)
{
    if(!create_dir(CACHE_FOLDER)) 
        log_l("Could not create required cache folder.", CRITICAL);
    return true;
}


// ==== Interface ====
const Cache *load_cache()
{
    printf("LOADING CACHE");
    fileStat cacheStat;

    // first attempt, if error, try to create cache folder
    if(!stat_path(CACHE_FOLDER, &cacheStat))
        create_cache();
    
    // second attempt, if failure, then do without cache
    if(!stat_path(CACHE_FOLDER, &cacheStat))
    {
        log_fatal("Could not load or create pipe cache. Try running atomically.", CACHE);
        return NULL;
    }

    static_cache.opened = true;
    return &static_cache;
}

void close_cache()
{
    if(static_cache.opened)
        static_cache.opened = false;
    return;
}

void clear_cache()
{
    return;
}
