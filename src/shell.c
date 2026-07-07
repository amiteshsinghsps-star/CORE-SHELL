/* shell.c — Read-Eval-Print Loop */
#include "shell.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "executor.h"
#include "history.h"
#include "readline_mod.h"
#include "jobs.h"
#include "signals.h"
#include "audit.h"
#include "restrict.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>

History shell_history;

void shell_init(int restricted) {
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);

        signals_init();

        shell_pgid = getpid();
        if (setpgid(shell_pgid, shell_pgid) < 0) {
            perror("Couldn't put shell in its own process group");
            exit(EXIT_FAILURE);
        }

        tcsetpgrp(shell_terminal, shell_pgid);
        tcgetattr(shell_terminal, &shell_tmodes);
    }

    history_init(&shell_history, HISTORY_MAX);
    history_load(&shell_history, ".coreshell_history");
    env_init();
    jobs_init();

    if (restricted) restrict_init();
}

void shell_loop(void) {
    char *line;
    Token *tokens;
    ASTNode *ast;

    while (1) {
        jobs_notify();
        line = readline_read(build_prompt());
        if (!line) break;
        if (*line == '\0') { free(line); continue; }

        history_add(&shell_history, line);
        tokens = lexer_tokenize(line);
        if (!tokens) { free(line); continue; }

        ast = parser_parse(tokens);
        if (!ast) { tokens_free(tokens); free(line); continue; }

        expand_ast(ast);
        audit_log(line);
        executor_run(ast);

        ast_free(ast);
        tokens_free(tokens);
        free(line);
    }
}
