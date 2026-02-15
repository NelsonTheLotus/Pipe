#!/bin/bash

# folders
mkdir -p Build/objects 2>/dev/null

# execute
mkdir -p Build/objects/execute 2>/dev/null
gcc -c Source/execute/scheduler.c -o Build/objects/execute/scheduler.o
gcc -c Source/execute/worker.c -o Build/objects/execute/worker.o
gcc -c Source/execute/shell.c -o Build/objects/execute/shell.o

# load
# mkdir -p Build/objects/load 2>/dev/null
# gcc -c Source/load/cache.c -o Build/objects/load/cache.o

# process

# read

# util
mkdir -p Build/objects/util 2>/dev/null
gcc -c Source/util/log.c -o Build/objects/util/log.o
gcc -c Source/util/platform.c -o Build/objects/util/platform.o
gcc -c Source/util/terminal.c -o Build/objects/util/terminal.o

# main
gcc -c Source/main.c -o Build/objects/main.o

gcc \
Build/objects/execute/scheduler.o \
Build/objects/execute/worker.o \
Build/objects/execute/shell.o \
Build/objects/util/log.o \
Build/objects/util/platform.o \
Build/objects/util/terminal.o \
Build/objects/main.o \
-o Build/pipe