#include "shell.h"

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>



// INTERNAL

static bool waitpid_timeout(pid_t pid, unsigned int timeout, int* status)
{
    time_t start = time(NULL);
    pid_t retpid = 0;

    while(!(retpid = waitpid(pid, &status, WNOHANG)))
    {
        if(time(NULL) - start >= timeout) break;
    }

    if(retpid > 0) return true;
    
    *status = errno;
    return false;
}



// INTERFACE

Shell shell_init()
{
    Shell ret_shell = {
        .pid = -1,
        .input_pipe = -1,
        .output_pipe = -1,
        .halted = false,
        .exit_code = 0
    };

    return ret_shell;
}


bool shell_start(Shell* s)
{
    if(s == NULL) return false;
    if(s->pid != -1) return true;

    // create pipes
    int to_shell[2]; pipe(to_shell);
    int from_shell[2]; pipe(from_shell);
    int shell_err[2]; pipe(shell_err);

    s->pid = fork();

    if(s->pid == 0)     // child fork
    {
        close(to_shell[1]);     // close write end (avoid hanging read calls)
        close(from_shell[0]);   // close read end
        close(shell_err[0]);    // child is writting error

        dup2(to_shell[0], STDIN_FILENO);
        dup2(from_shell[1], STDOUT_FILENO);
        dup2(from_shell[1], STDERR_FILENO);

        fcntl(shell_err[1], F_SETFD, FD_CLOEXEC);   // Close write on exec success
        setpgid(0, 0);                              // make leader of a new process group ID
        execl("/bin/sh", "sh", NULL);               // does not continue if exec suceeds

        // on exec failure
        int err = errno;
        write(shell_err[1], &err, sizeof(err));
        _exit(1);                                   // kill forked process, async-signal-safe
    }

    // parent fork
    close(to_shell[0]);     // close read end
    close(from_shell[1]);   // close write end (avoid hanging read calls)
    close(shell_err[1]);    // parent reads err

    s->input_pipe = to_shell[1];
    s->output_pipe = from_shell[0];
    s->halted = false;      // weather running or not, not halted

    // check that forked process spawned shell
    int err;
    size_t bytes = read(shell_err[0], &err, sizeof(err));
    close(shell_err[0]);
    if(bytes == 0) return true; // success

    // child failure
    s->exit_code = err;
    waitpid(s->pid, NULL, 0);
    s->pid = -1;

    close(s->input_pipe);
    close(s->output_pipe);
    s->input_pipe = -1;
    s->output_pipe = -1;

    // no log, since inside thread
    return false;
}


void shell_halt(Shell* s)
{
    if(s == NULL) return;
    if(s->pid == -1) return;
    kill(s->pid, SIGSTOP);  // halt execution
    s->halted = true;
    return;
}


void shell_resume(Shell* s)
{
    if(s == NULL) return;
    if(s->pid == -1) return;
    kill(s->pid, SIGCONT);  // continue execution
    s->halted = false;
    return;
}


bool shell_stop(Shell* s, unsigned int timeout)
{
    if(s == NULL) return true;
    if(s->pid == -1) return true;

    write(s->input_pipe, "exit\n", 5);
    return waitpid_timeout(s->pid, timeout, &s->exit_code);
}


void shell_kill(Shell* s, int timeout)
{
    if(s == NULL) return;
    if(s->pid == -1) return;

    if(timeout > 0)
    {
        kill(s->pid, SIGTERM);
        if(waitpid_timeout(s->pid, timeout, &s->exit_code))
            return;
    }
    
    kill(-s->pid, SIGKILL);                 // send kill to gpid
    waitpid(-s->pid, &s->exit_code, 0);
    return;
}
