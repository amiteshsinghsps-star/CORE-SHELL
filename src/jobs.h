/* jobs.h */
#pragma once
#include <termios.h>
#include <sys/types.h>

#define MAX_JOBS   64
#define JOB_RUN    0
#define JOB_STOP   1
#define JOB_DONE   2

typedef struct Proc {
    pid_t        pid;
    char        *cmd;
    int          status;     /* last waitpid status */
    int          stopped;
    int          completed;
    /* cgroups integration */
    int          has_cgroup;
    char         cgroup_path[256];
    struct Proc *next;
} Proc;

typedef struct Job {
    int          jid;        /* 1-based job number */
    pid_t        pgid;       /* process group id */
    Proc        *procs;
    char        *cmdline;
    int          state;      /* JOB_RUN / JOB_STOP / JOB_DONE */
    int          foreground;
    struct termios tmodes;
    /* resource limits applied */
    long         mem_limit_mb;
    int          cpu_weight;
    /* namespace flags */
    int          ns_flags;
    struct Job  *next;
} Job;

extern Job     *job_table;
extern pid_t    shell_pgid;
extern struct termios shell_tmodes;
extern int      shell_terminal;

void jobs_init(void);
void jobs_notify(void);
Job *job_new(pid_t pgid, const char *cmdline);
void job_wait_foreground(Job *job);
void job_remove_if_done(Job *job);
void job_update_status(pid_t pid, int status);
void jobs_kill_all(void);
int job_fg(int jid);
int job_bg(int jid);
int jobs_last_stopped(void);
