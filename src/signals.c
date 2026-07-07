/* signals.c — Unified signal handling */
#include "signals.h"
#include "jobs.h"
#include "audit.h"
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

static void handle_sigchld(int sig);
static void handle_sigterm(int sig);

void signals_init(void) {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGTTIN, &sa, NULL);
    sigaction(SIGTTOU, &sa, NULL);

    sa.sa_handler = handle_sigchld;
    sa.sa_flags |= SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    sa.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa, NULL);
}

void signals_reset_child(void) {
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGTTIN, &sa, NULL);
    sigaction(SIGTTOU, &sa, NULL);
    sigaction(SIGCHLD, &sa, NULL);
}

static void handle_sigchld(int sig) {
    (void)sig;
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED)) > 0) {
        job_update_status(pid, status);
    }
}

static void handle_sigterm(int sig) {
    (void)sig;
    jobs_kill_all();
    audit_close();
    _exit(0);
}
