#!/bin/bash

# folders
mkdir -p Build/objects 2>/dev/null

# parser
mkdir -p Build/objects/parser 2>/dev/null
gcc -c Source/parser/cli.c -o Build/objects/parser/cli.o


# main
gcc -c Source/main.c -o Build/objects/main.o

gcc Build/objects/parser/cli.o Build/objects/main.o -o Build/pipe