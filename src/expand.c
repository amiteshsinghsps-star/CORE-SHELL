/* expand.c - Expansion */
#include "expand.h"
#include <stdlib.h>
#include <string.h>

void expand_ast(ASTNode *node) {
    if (!node) return;
    
    if (node->type == AST_COMMAND) {
        for (int i = 0; i < node->argc; i++) {
            if (node->argv[i][0] == '$') {
                char *env_val = getenv(node->argv[i] + 1);
                free(node->argv[i]);
                node->argv[i] = env_val ? strdup(env_val) : strdup("");
            }
        }
    } else if (node->type == AST_PIPELINE || node->type == AST_SEQUENCE || node->type == AST_AND || node->type == AST_OR) {
        expand_ast(node->left);
        expand_ast(node->right);
    } else if (node->type == AST_REDIRECT) {
        expand_ast(node->left);
    } else if (node->type == AST_BACKGROUND) {
        expand_ast(node->child);
    }
}
