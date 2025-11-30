#pragma once

#define PROCESS_PUBLIC

#include "dependency.h" // to check dependencies and flag recompilation
#include "graph.h"      // to graph the job/dependency tree for job order
#include "scheduler.h"  // to schedule jobs following graph and direct parralelism

#undef PROCESS_PUBLIC