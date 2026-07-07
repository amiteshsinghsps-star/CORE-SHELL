/* audit.c — Security audit trail */
#include "audit.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

static FILE *audit_fp = NULL;
static pid_t shell_pid;

int audit_init(const char *logfile) {
    audit_fp = fopen(logfile, "a");
    if (!audit_fp) { perror("audit_init"); return -1; }
    shell_pid = getpid();
    time_t now = time(NULL);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", localtime(&now));
    fprintf(audit_fp, "--- SESSION START: pid=%d user=%s time=%s ---\n",
            shell_pid, getlogin() ? getlogin() : "unknown", ts);
    fflush(audit_fp);
    return 0;
}

void audit_log(const char *cmd) {
    if (!audit_fp) return;
    time_t now = time(NULL);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", localtime(&now));
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    fprintf(audit_fp, "[%s] pid=%d uid=%d cwd=%s cmd=%s\n",
            ts, (int)getpid(), (int)getuid(), cwd, cmd);
    fflush(audit_fp);
}

void audit_close(void) {
    if (!audit_fp) return;
    time_t now = time(NULL);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", localtime(&now));
    fprintf(audit_fp, "--- SESSION END: time=%s ---\n", ts);
    fflush(audit_fp);
    fclose(audit_fp);
    audit_fp = NULL;
}
