#pragma once

// Pass commands to scheduler, which feeds commands to shell.

#define EXECUTE_PUBLIC
#include "command.h"
#include "scheduler.h"
// do not include: worker.h
// do not include: shell.h
#undef EXECUTE_PUBLIC