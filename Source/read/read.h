#pragma once

#define READ_PUBLIC

#include "lexer.h"      // to check syntax validity
#include "parser.h"     // to read, validate and transform pipefile into command list
#include "tokenizer.h"  // to tokenize the text

#undef READ_PUBLIC