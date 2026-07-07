/* jobs.c - Job Control */
#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

Job *job_table = NULL;

void jobs_init(void) {
    job_table = NULL;
}

Job *job_new(pid_t pgid, const char *cmdline) {
    Job *j = calloc(1, sizeof(Job));
    j->pgid = pgid;
    j->cmdline = strdup(cmdline);
    j->state = JOB_RUNNING;
    
    int max_id = 0;
    Job *curr = job_table;
    while (curr) {
        if (curr->job_id > max_id) max_id = curr->job_id;
        curr = curr->next;
    }
    j->job_id = max_id + 1;
    
    Process *p = calloc(1, sizeof(Process));
    p->pid = pgid;
    p->cmdline = strdup(cmdline);
    j->procs = p;
    
    j->next = job_table;
    job_table = j;
    return j;
}

void job_update_status(pid_t pid, int status) {
    Job *curr = job_table;
    while (curr) {
        Process *p = curr->procs;
        while (p) {
            if (p->pid == pid) {
                p->status = status;
                if (WIFSTOPPED(status)) {
                    p->stopped = 1;
                    curr->state = JOB_STOPPED;
                } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    p->completed = 1;
                    curr->state = JOB_DONE;
                }
                return;
            }
            p = p->next;
        }
        curr = curr->next;
    }
}

void job_wait_foreground(Job *job) {
    int status;
    waitpid(job->pgid, &status, WUNTRACED);
    job_update_status(job->pgid, status);
    if (WIFSTOPPED(status)) {
        printf("\n[%d]+ Stopped    %s\n", job->job_id, job->cmdline);
        tcgetattr(STDIN_FILENO, &job->tmodes);
    }
}

void job_remove_if_done(Job *job) {
    if (job->state != JOB_DONE) return;
    
    Job **curr = &job_table;
    while (*curr) {
        if (*curr == job) {
            *curr = job->next;
            Process *p = job->procs;
            while (p) {
                Process *next = p->next;
                free(p->cmdline);
                free(p);
                p = next;
            }
            free(job->cmdline);
            free(job);
            return;
        }
        curr = &(*curr)->next;
    }
}

void jobs_notify(void) {
    Job **curr = &job_table;
    while (*curr) {
        Job *j = *curr;
        if (j->state == JOB_DONE) {
            printf("[%d]  Done       %s\n", j->job_id, j->cmdline);
            *curr = j->next;
            Process *p = j->procs;
            while (p) {
                Process *next = p->next;
                free(p->cmdline);
                free(p);
                p = next;
            }
            free(j->cmdline);
            free(j);
        } else {
            curr = &(*curr)->next;
        }
    }
}

void jobs_kill_all(void) {
    Job *curr = job_table;
    while (curr) {
        kill(-curr->pgid, SIGTERM);
        curr = curr->next;
    }
}

int job_fg(int jid) {
    Job *curr = job_table;
    while (curr) {
        if (curr->job_id == jid) {
            curr->state = JOB_RUNNING;
            tcsetpgrp(STDIN_FILENO, curr->pgid);
            if (curr->state == JOB_STOPPED || curr->procs->stopped) {
                tcsetattr(STDIN_FILENO, TCSADRAIN, &curr->tmodes);
                kill(-curr->pgid, SIGCONT);
            }
            job_wait_foreground(curr);
            tcsetpgrp(STDIN_FILENO, shell_pgid);
            job_remove_if_done(curr);
            return 0;
        }
        curr = curr->next;
    }
    fprintf(stderr, "fg: job not found: %d\n", jid);
    return 1;
}

int job_bg(int jid) {
    Job *curr = job_table;
    while (curr) {
        if (curr->job_id == jid) {
            curr->state = JOB_RUNNING;
            printf("[%d] %s &\n", curr->job_id, curr->cmdline);
            kill(-curr->pgid, SIGCONT);
            return 0;
        }
        curr = curr->next;
    }
    fprintf(stderr, "bg: job not found: %d\n", jid);
    return 1;
}

int jobs_last_stopped(void) {
    int last = 0;
    Job *curr = job_table;
    while (curr) {
        if (curr->state == JOB_STOPPED && curr->job_id > last)
            last = curr->job_id;
        curr = curr->next;
    }
    return last > 0 ? last : 1;
}
