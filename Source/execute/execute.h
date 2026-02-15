#pragma once

// Pass command to run, which feeds command to shell.
// Run manages threads and shells

#define EXECUTE_PUBLIC
#include "scheduler.h"
#include "worker.h"
#include "shell.h"
#undef EXECUTE_PUBLIC