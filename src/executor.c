/* executor.c — Core execution engine */
#include "executor.h"
#include "builtins.h"
#include "pipeline.h"
#include "redirect.h"
#include "jobs.h"
#include "signals.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

static int exec_command(ASTNode *node);

int executor_run(ASTNode *node) {
    if (!node) return 0;

    switch (node->type) {
        case AST_COMMAND:   return exec_command(node);
        case AST_PIPELINE:  return exec_pipeline(node);
        case AST_SEQUENCE:
            executor_run(node->left);
            return executor_run(node->right);
        case AST_AND:
            if (executor_run(node->left) == 0)
                return executor_run(node->right);
            return 1;
        case AST_OR:
            if (executor_run(node->left) != 0)
                return executor_run(node->right);
            return 0;
        case AST_BACKGROUND: return exec_background(node->child);
        case AST_REDIRECT:   return exec_redirect(node);
        default: return -1;
    }
}

static int exec_command(ASTNode *node) {
    if (is_builtin(node->argv[0]))
        return run_builtin(node->argv);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return -1; }

    if (pid == 0) {
        setpgid(0, 0);
        signals_reset_child();
        execvp(node->argv[0], node->argv);
        fprintf(stderr, "coreshell: %s: command not found\n", node->argv[0]);
        exit(127);
    }

    setpgid(pid, pid);
    Job *job = job_new(pid, node->argv[0]);
    job->foreground = 1;

    tcsetpgrp(STDIN_FILENO, pid);
    job_wait_foreground(job);
    tcsetpgrp(STDIN_FILENO, shell_pgid);

    int status = job->procs->status;
    job_remove_if_done(job);
    return WEXITSTATUS(status);
}
