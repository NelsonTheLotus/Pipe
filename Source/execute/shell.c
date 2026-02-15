#include "shell.h"

#include "../util/util.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>


Shell create_shell(void)
{
    // creates pipes
    int write_pipe[2];
    int read_pipe[2];
    int err_pipe[2];
    pipe(write_pipe);
    pipe(read_pipe);
    pipe(err_pipe);

    // create the new shell
    Shell newShell = {0};
    newShell.shell_pid = fork();

    // handle forked child
    if(newShell.shell_pid == 0)
    {
        dup2(write_pipe[0], STDIN_FILENO);
        dup2(read_pipe[1], STDOUT_FILENO);
        dup2(read_pipe[1], STDERR_FILENO);

        close(write_pipe[1]);   // avoid hanging reads because write is still open
        close(read_pipe[0]);    // clean unused reading point
        close(err_pipe[0]);     // clean unused reading point
        
        fcntl(err_pipe[1], F_SETFD, FD_CLOEXEC);    // Close write on exec success
        setpgid(0, 0);  // make leader of a new process group ID
        execl("/bin/sh", "sh", NULL);

        // on exec failure
        int err = errno;
        write(err_pipe[1], &err, sizeof(err));
        _exit(1);    // kill forked process, async-signal-safe
    }

    // handle parent process
    close(write_pipe[0]);
    close(read_pipe[1]);
    close(err_pipe[1]);

    newShell.shell_input = write_pipe[1];
    newShell.shell_output = read_pipe[0];

    // check that forked process spawned shell
    int err;
    size_t bytes = read(err_pipe[0], &err, sizeof(err));
    close(err_pipe[0]);
    if(bytes == 0) return newShell; // success

    // child failure
    newShell.err_code = err;
    waitpid(newShell.shell_pid, NULL, 0);
    newShell.shell_pid = -1;

    close(newShell.shell_input);
    close(newShell.shell_output);
    newShell.shell_input = -1;
    newShell.shell_output = -1;

    log_fatal("failed to open shell.", SYSTEM); //TODO: no log fatal, just report error, scheduler will manage.
    return newShell;
    // no log, since inside thread
}


// returns -1 on error (errno = 0 -> timeout), exit code otherwise
static int waitpid_timeout(pid_t pid, unsigned int timeout)
{
    time_t start = time(NULL);
    int status;
    pid_t retpid = 0;
    while(retpid == 0)
    {
        retpid = waitpid(pid, &status, WNOHANG);
        if(retpid == -1) return -1;
        if(time(NULL) - start >= timeout)
        {
            errno = 0;
            return -1;
        }   // timeout
    }

    return status;
}


int destroy_shell(Shell* shell, bool force)
{
    if(shell == NULL) return 0;
    int ret = 0;
    if(!force)
    {
        const char* exit_cmd = "exit\n";
        write(shell->shell_input, exit_cmd, strlen(exit_cmd));
        close(shell->shell_input);
        ret = waitpid_timeout(shell->shell_pid, GRACEFUL_TIMEOUT);
    }

    if(ret >= 0) return ret;
    if(ret == -1 && errno != 0) return errno;
    kill(-(shell->shell_pid), SIGTERM);  // graceful force
    ret = waitpid_timeout(shell->shell_pid, FORCEFUL_TIMEOUT);

    if(ret >= 0) return ret;
    if(ret == -1 && errno != 0) return errno;
    kill(-(shell->shell_pid), SIGKILL);
    waitpid(shell->shell_pid, &ret, 0);
    return ret;
}