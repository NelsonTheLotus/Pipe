

## Variables
use `!` for built-ins, `$` for user-defined

## Actions
Each tag defines an action (e.g. compile, link, build, clean)
Each line defines a step




I'll go with pipe. I'd like to get to work on a minimal working syntax subset. I'm thinking the features I need are:
1. Define what commands should be run and how (command + flags)
2. Define input files
3. Where to put the compiled outputs, internationally to the file inputs
4. How and what to link

Questions:
1. Is there anything missing for the most basic feature set
2. How to implement cross language? It should be generic enough that any form of sequencial file processing should be achievable using it
3. How to implement cross-platform, especially concerning standard libs and file location, as well as potential platform-specific flags and so on
4. How to implement (or at least future-proof) for multi-core compilation?

+ Path normalization: internally canonicalize paths to avoid duplication (./a.c vs a.c).
+ Atomic write: ensure commands produce temporary files and then atomically move them to destination (avoid half-written outputs on failure).
+ Minimal diagnostics: timestamps, last-run hashes, and a readable build trace.
+ Error provenance: store the exact command and environment used for a file so a retry is repeatable.
+ Plugin hooks (later): pre/post hooks for each step (useful, but not essential now).