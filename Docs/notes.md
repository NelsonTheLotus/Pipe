pipe creates a .pipe folder where it stores all it's metadata.
Metadata includes:
- File comp time and hash (for dependency evaluation)
- Logs for previous executions
- Stored configuration/pipe file for easy recompilation without having to reconfigure (pipe cache)

Jobs is the combination of currently executing commands. If a thread is running 1 command that 1 job.
If 2 threads are running 2 commands each, that's 4 jobs. Pipe decides the optimal order in which
to issue commands.

within a mapping, if doing wildcard match, allow an option to ignore matched files that have
been processed within the same mapping (default). Or deactivate the option if not. 
Also allow regex file searches.

DO NOT forget to check for dependencies regarding modification of pipefile, using cache.

Add triggers in the config, for example an onClear trigger, run this pipefile rule:
```
config
{
    onClear: clear  // flow called clear
}
```
