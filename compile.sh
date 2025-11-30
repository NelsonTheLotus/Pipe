#!/bin/bash

# folders
mkdir -p Build/objects 2>/dev/null

# util
mkdir -p Build/objects/util 2>/dev/null
gcc -c Source/util/cli.c -o Build/objects/util/cli.o
gcc -c Source/util/error.c -o Build/objects/util/error.o


# main
gcc -c Source/main.c -o Build/objects/main.o

gcc Build/objects/util/cli.o Build/objects/main.o -o Build/pipe