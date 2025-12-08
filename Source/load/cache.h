#pragma once

#include "../global.h"


// ==== Public structures ====

typedef struct Cache
{
    bool opened;    // weather the cache was opened for reading
} Cache;

// ==== Interface ====

//* Load the cache
const Cache *load_cache();

//* When finnished, close
void close_cache(void);

//* Deletes curent cache; This is non-reversible
void clear_cache();

//* Log cache statuses
void log_status(const char* status_array);
