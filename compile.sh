#!/bin/bash

# folders
mkdir -p Build/objects 2>/dev/null

# parser
mkdir -p Build/objects/parser 2>/dev/null
g++ -c Source/parser/cli.cpp -o Build/objects/parser/cli.o


# main
g++ -c Source/main.cpp -o Build/objects/main.o

g++ Build/objects/parser/cli.o Build/objects/main.o -o Build/pipe