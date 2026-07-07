/* script.c */
#include "script.h"
#include "shell.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int shell_run_command(const char *cmd) {
    Token *tokens = lexer_tokenize(cmd);
    if (!tokens) return 0;
    ASTNode *ast = parser_parse(tokens);
    if (!ast) { tokens_free(tokens); return 0; }
    expand_ast(ast);
    int status = executor_run(ast);
    ast_free(ast);
    tokens_free(tokens);
    return status;
}

int shell_run_script(const char *script_file) {
    FILE *f = fopen(script_file, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '#' || line[0] == '\0') continue;
        shell_run_command(line);
    }
    
    fclose(f);
    return 0;
}
