#include "cache.h"

#include "../global.h"
#include "../util/util.h"


// ==== Internal structures ====


// ==== Static elements ====


// ==== Internal Helpers ====

//* Create the directory and files for the cache
void create_cache();


// ==== Interface ====
Cache *load_cache();
void close_cache();
void clear_cache();
