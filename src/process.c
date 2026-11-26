#define _XOPEN_SOURCE 700

#include "process.h"

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/wait.h>

static pid_t traced_pid;
static int pending_signal;

static pid_t wait_for_child(pid_t pid, int *status, int options) {
    pid_t result;
    do {
        result = waitpid(pid, status, options);
    } while (result == -1 && errno == EINTR);
    return result;
}

static void show_stop(pid_t pid, int signal) {
    printf("Stopped PID %ld by signal %d\n", (long) pid, signal);
}

static void show_termination(int status) {
    if (WIFEXITED(status)) {
        printf("Exit code %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Terminated by signal %d\n", WTERMSIG(status));
    }
}

static void launch_program(const char *path, bool trace) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        if (trace && ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("ptrace TRACEME");
            _exit(1);
        }
        execl(path, path, (char *) NULL);
        perror("execl");
        _exit(1);
    }

    int status;
    if (wait_for_child(pid, &status, 0) == -1) {
        perror("waitpid");
        return;
    }
    if (trace && WIFSTOPPED(status)) {
        int signal = WSTOPSIG(status);
        traced_pid = pid;
        /* The exec SIGTRAP is the debugger's initial stop. */
        pending_signal = signal == SIGTRAP ? 0 : signal;
        show_stop(pid, signal);
        return;
    }
    show_termination(status);
}

void run_program(const char *path) {
    launch_program(path, false);
}

void debug_program(const char *path) {
    if (traced_pid != 0) {
        puts("A program is already being debugged.");
        return;
    }
    launch_program(path, true);
}

static bool is_step_trap(int signal) {
    if (signal != SIGTRAP) {
        return false;
    }

    siginfo_t info;
    if (ptrace(PTRACE_GETSIGINFO, traced_pid, NULL, &info) == -1) {
        perror("ptrace GETSIGINFO");
        return false;
    }
    return info.si_code == TRAP_TRACE;
}

static void resume_program(bool step) {
    if (traced_pid == 0) {
        puts(step ? "No program to step." : "No program to continue.");
        return;
    }

    int status;
    if (ptrace(step ? PTRACE_SINGLESTEP : PTRACE_CONT, traced_pid,
               NULL, (void *) (intptr_t) pending_signal) == -1) {
        int error = errno;
        perror(step ? "ptrace SINGLESTEP" : "ptrace CONT");
        if (error == ESRCH) {
            pid_t result = wait_for_child(traced_pid, &status, WNOHANG);
            if (result > 0 && (WIFEXITED(status) || WIFSIGNALED(status))) {
                traced_pid = 0;
                show_termination(status);
            } else if (result == -1 && errno == ECHILD) {
                traced_pid = 0;
            }
        }
        return;
    }
    if (wait_for_child(traced_pid, &status, 0) == -1) {
        int error = errno;
        perror("waitpid");
        if (error == ECHILD) {
            traced_pid = 0;
        }
        return;
    }
    if (WIFSTOPPED(status)) {
        int signal = WSTOPSIG(status);
        pending_signal = step && is_step_trap(signal) ? 0 : signal;
        show_stop(traced_pid, signal);
    } else {
        traced_pid = 0;
        show_termination(status);
    }
}

void continue_program(void) {
    resume_program(false);
}

void step_program(void) {
    resume_program(true);
}

void close_debug_session(void) {
    if (traced_pid == 0) {
        return;
    }
    if (kill(traced_pid, SIGKILL) == -1 && errno != ESRCH) {
        perror("kill");
        return;
    }
    if (wait_for_child(traced_pid, NULL, 0) == -1 && errno != ECHILD) {
        perror("waitpid");
    }
    traced_pid = 0;
}
