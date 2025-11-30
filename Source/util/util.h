#pragma once

#define UTIL_PUBLIC

#include "cli.h"        // to interface with the CLI (options parsing, ...)
#include "log.h"        // to log the process
#include "error.h"      // to generate errors
#include "platform.h"   // to have platform-(in)dependent code

#undef UTIL_PUBLIC
