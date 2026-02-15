#include "worker.h"

#include <stddef.h>


Worker new_worker(size_t workerID)
{
    Shell newShell = (Shell){0};
    return (Worker){workerID, &(Shell){0}};
}


void close_worker(Worker* worker)
{
    close_shell(worker->executor);
}