#pragma once


// Public structures

typedef struct Cache
{

} Cache;

// Interface

//* Load the cache
Cache *load_cache();
//* When finnished, close
void close_cache();
//* Deletes curent cache; This is non-reversible
void clear_cache();