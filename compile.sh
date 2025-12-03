#!/bin/bash

# folders
mkdir -p Build/objects 2>/dev/null

# execute

# load
mkdir -p Build/objects/load 2>/dev/null
gcc -c Source/load/cache.c -o Build/objects/load/cache.o

# process

# read

# util
mkdir -p Build/objects/util 2>/dev/null
gcc -c Source/util/cli.c -o Build/objects/util/cli.o
gcc -c Source/util/log.c -o Build/objects/util/log.o
gcc -c Source/util/platform.c -o Build/objects/util/platform.o

# main
gcc -c Source/main.c -o Build/objects/main.o

gcc \
Build/objects/load/cache.o \
Build/objects/util/cli.o \
Build/objects/util/log.o \
Build/objects/util/platform.o \
Build/objects/main.o \
-o Build/pipe