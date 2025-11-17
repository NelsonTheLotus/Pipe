========================================
Pipe developpment decisions and features
========================================

This file references the developpment features and ideas added to pipe.

-----------------------
1. Command line options
-----------------------

1.1 Options
___________
- Help: prints help
- Config: Rerun file config block, ignoring cache and emitting generated artefacts. Automatically run if pipefile is newer than cache.
- Status: Print various cache states and statuses
- Clear: Clear cache
- Atomic: Run pipe with no cache. Any cache state will hence be ignored and not emitted.
- Verbose: Print some extra logs
- Parse: Only parse and validate pipe file
- Define: Define new varaibles.
- Jobs: Maximum permitted jobs; default 1
- File: input file to read. If not provided, looks for './pipefile'

1.2 Parsing
___________

Once passed, command line options, henceforth called arguments, are parsed according to the following rules:
a. If argument is not preceded by a ('-')tack, it is considered a flow to run
b. If argument has a single ('-')tack, argument is expected to ba a single letter. Anything accuring after 
that single character will be considered an argument value.
c. If argument CAN take an argument AND [argument has double ('-')tack OR if single-tack argument has no assigned value],
next argument is checked.
- If next argument does not have a tack, it will be considered an argument value.
- If it does, it will be parsed as the next option.

When passing an option but not wanting to pass a value for said option, one can simply replace the value with a '.' dot.


1.3 Interpretation and execution
________________________________

By order of priority:

- If a help, status or clear option is passed, all other options are ignored.
- If help and/or status option is passed, both are run as help -> status
- If help and/or clear option is passed, both are run as help -> clear
- If a status and clear option is passed, only status is run, clear is ignored.
- If a parse option is passed, all flows will be ignored. Then, program basically creates a "dry-run" of the pipefile.
- All other options may be defined simultanously.