/* pipeline.c - Pipeline Execution */
#include "pipeline.h"
#include "executor.h"
#include "jobs.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int exec_pipeline(ASTNode *node) {
    if (!node || node->type != AST_PIPELINE) return -1;
    
    int pipes[2];
    if (pipe(pipes) < 0) {
        perror("pipe");
        return -1;
    }
    
    pid_t left_pid = fork();
    if (left_pid < 0) {
        perror("fork");
        return -1;
    }
    
    if (left_pid == 0) {
        dup2(pipes[1], STDOUT_FILENO);
        close(pipes[0]);
        close(pipes[1]);
        exit(executor_run(node->left));
    }
    
    pid_t right_pid = fork();
    if (right_pid < 0) {
        perror("fork");
        return -1;
    }
    
    if (right_pid == 0) {
        dup2(pipes[0], STDIN_FILENO);
        close(pipes[0]);
        close(pipes[1]);
        exit(executor_run(node->right));
    }
    
    close(pipes[0]);
    close(pipes[1]);
    
    int status;
    waitpid(left_pid, NULL, 0);
    waitpid(right_pid, &status, 0);
    
    return WEXITSTATUS(status);
}

int exec_background(ASTNode *node) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        setpgid(0, 0);
        exit(executor_run(node));
    }
    setpgid(pid, pid);
    Job *job = job_new(pid, "background job");
    job->foreground = 0;
    printf("[%d] %d\n", job->job_id, pid);
    return 0;
}
