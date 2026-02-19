#include "shell.h"

#include "../util/util.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>


Shell new_shell(void)
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


// close open shell endpoints
static void close_shell(Shell* shell)
{
    shell->shell_pid = -1;
    close(shell->shell_input);
    close(shell->shell_output);

    return;
}

int stop_shell(Shell* shell, bool force)
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

    if(ret >= 0) {close_shell(shell); return ret;}
    if(ret == -1 && errno != 0) {close_shell(shell); return errno;}
    kill(-(shell->shell_pid), SIGTERM);  // graceful force
    ret = waitpid_timeout(shell->shell_pid, FORCEFUL_TIMEOUT);

    if(ret >= 0) {close_shell(shell); return ret;}
    if(ret == -1 && errno != 0) {close_shell(shell); return errno;}
    kill(-(shell->shell_pid), SIGKILL);
    waitpid(shell->shell_pid, &ret, 0);
    close_shell(shell);
    return ret;
}


// internal helper
/* CommandResult capture_output(Shell* shell, const char* cmd_delimiter)
{
    char* read_buf = (char*)malloc(4096*sizeof(char));

    bool cmd_done = false;
    int chars_read = 0;
    while(!cmd_done)
    {
        chars_read = read(shell->shell_output, read_buf, 4096);
        if(chars_read == 0) break;  // EOF reached
        if(chars_read == -1) break; // ERROR

        char* line_start = read_buf;
        char* newline_ptr = strchr(line_start, '\n');   // returns ptr to '\n'
        if(newline_ptr == NULL) continue;
        while(!strncmp(cmd_delimiter, line_start, newline_ptr-line_start))
        {
            line_start = newline_ptr+1;
            newline_ptr = strchr(line_start, '\n');
        }

    }

    //read a buffer
    // parse for delimiter
    // if no delimiter found 
    // read_buffer
} */


CommandResult issue_command(Shell* shell, ShellCommand command)
{
    log_msg("Executing command: %s", command.cmd);
    fflush(stdout);
    write(shell->shell_input, command.cmd, strlen(command.cmd));
    write(shell->shell_input, "\n", 1);

    size_t echo_len = sizeof(char)*(strlen(command.id_glob)+6);
    char* echo_buf = (char*)malloc(echo_len);  // strlen + "echo " + \0
    // TODO: graceful thread failure on fail
    snprintf(echo_buf, echo_len, "echo %s", command.id_glob);

    write(shell->shell_input, echo_buf, echo_len);
    write(shell->shell_input, "\n", 1);
    printf("File descriptor is %d\n", shell->shell_input);
    
    //CommandResult result = capture_output(shell, command.id_glob);
    CommandResult result;
    read(shell->shell_output, result.output_buf, 4096);
    printf("Output: %s\n", result.output_buf);
    return result;
}