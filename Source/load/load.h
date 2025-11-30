#pragma once

#define LOAD_PUBLIC

#include "cache.h"      // to load and manage cache
#include "config.h"     // to load and parse configuration
#include "context.h"    // to allow access to external vars (environnment, terminal, ...)
#include "state.h"      // to track the state of the cache and config

#undef LOAD_PUBLIC