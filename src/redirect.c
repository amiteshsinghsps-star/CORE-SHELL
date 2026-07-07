/* redirect.c - I/O Redirection */
#include "redirect.h"
#include "executor.h"
#include "restrict.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int exec_redirect(ASTNode *node) {
    if (!node || node->type != AST_REDIRECT) return -1;
    
    if (restrict_check("redirect", NULL, node->redir_file) != 0) return -1;
    
    int fd;
    if (node->redir_type == TOKEN_REDIR_OUT) {
        fd = open(node->redir_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); return -1; }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    } else if (node->redir_type == TOKEN_REDIR_APPEND) {
        fd = open(node->redir_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) { perror("open"); return -1; }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    } else if (node->redir_type == TOKEN_REDIR_IN) {
        fd = open(node->redir_file, O_RDONLY);
        if (fd < 0) { perror("open"); return -1; }
        dup2(fd, STDIN_FILENO);
        close(fd);
    } else if (node->redir_type == TOKEN_REDIR_ERR) {
        fd = open(node->redir_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); return -1; }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    
    return executor_run(node->left);
}
