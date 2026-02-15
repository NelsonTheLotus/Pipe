#include "worker.h"

#include "shell.h"

#include "../util/util.h"

#include <stddef.h>


Worker new_worker(size_t workerID)
{
    Shell newShell = create_shell();
    if(newShell.shell_pid == -1)
        log_l("Failed to create executor shell.", CRITICAL);    // TODO: add more log info on success/failure (pid, errcode, ...)
    else log_l("Created new shell.", INFO);
    return (Worker){workerID, newShell};
}


void close_worker(Worker* worker)
{
    destroy_shell(&(worker->executor), false);
    log_l("Shell terminated.", INFO);   // TODO: add more info on which chell was terminated
}