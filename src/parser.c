/* parser.c - AST Parser */
#include "parser.h"
#include <stdlib.h>
#include <string.h>

static ASTNode *parse_sequence(Token **tokens);
static ASTNode *parse_pipeline(Token **tokens);
static ASTNode *parse_command(Token **tokens);

ASTNode *parser_parse(Token *tokens) {
    Token *curr = tokens;
    return parse_sequence(&curr);
}

static ASTNode *parse_sequence(Token **tokens) {
    if (!*tokens) return NULL;
    ASTNode *left = parse_pipeline(tokens);
    if (!left) return NULL;

    while (*tokens && ((*tokens)->type == TOKEN_SEMICOLON || (*tokens)->type == TOKEN_AND || (*tokens)->type == TOKEN_OR)) {
        ASTNode *node = calloc(1, sizeof(ASTNode));
        if ((*tokens)->type == TOKEN_SEMICOLON) node->type = AST_SEQUENCE;
        else if ((*tokens)->type == TOKEN_AND) node->type = AST_AND;
        else if ((*tokens)->type == TOKEN_OR) node->type = AST_OR;
        
        *tokens = (*tokens)->next;
        node->left = left;
        node->right = parse_pipeline(tokens);
        left = node;
    }
    return left;
}

static ASTNode *parse_pipeline(Token **tokens) {
    if (!*tokens) return NULL;
    ASTNode *left = parse_command(tokens);
    if (!left) return NULL;

    while (*tokens && (*tokens)->type == TOKEN_PIPE) {
        ASTNode *node = calloc(1, sizeof(ASTNode));
        node->type = AST_PIPELINE;
        *tokens = (*tokens)->next;
        node->left = left;
        node->right = parse_command(tokens);
        left = node;
    }
    return left;
}

static ASTNode *parse_command(Token **tokens) {
    if (!*tokens) return NULL;
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = AST_COMMAND;
    
    int capacity = 10;
    node->argv = malloc(capacity * sizeof(char *));
    node->argc = 0;

    while (*tokens && (*tokens)->type == TOKEN_WORD) {
        if (node->argc >= capacity - 1) {
            capacity *= 2;
            node->argv = realloc(node->argv, capacity * sizeof(char *));
        }
        node->argv[node->argc++] = strdup((*tokens)->value);
        *tokens = (*tokens)->next;
    }
    node->argv[node->argc] = NULL;
    
    while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN || (*tokens)->type == TOKEN_REDIR_OUT || (*tokens)->type == TOKEN_REDIR_APPEND || (*tokens)->type == TOKEN_REDIR_ERR)) {
        ASTNode *redir = calloc(1, sizeof(ASTNode));
        redir->type = AST_REDIRECT;
        redir->redir_type = (*tokens)->type;
        *tokens = (*tokens)->next;
        if (*tokens && (*tokens)->type == TOKEN_WORD) {
            redir->redir_file = strdup((*tokens)->value);
            *tokens = (*tokens)->next;
        }
        redir->left = node;
        node = redir;
    }

    if (*tokens && (*tokens)->type == TOKEN_BACKGROUND) {
        ASTNode *bg = calloc(1, sizeof(ASTNode));
        bg->type = AST_BACKGROUND;
        bg->child = node;
        *tokens = (*tokens)->next;
        node = bg;
    }

    return node;
}

void ast_free(ASTNode *node) {
    if (!node) return;
    if (node->type == AST_COMMAND) {
        for (int i = 0; i < node->argc; i++) {
            free(node->argv[i]);
        }
        free(node->argv);
    } else if (node->type == AST_REDIRECT) {
        free(node->redir_file);
        ast_free(node->left);
    } else if (node->type == AST_BACKGROUND) {
        ast_free(node->child);
    } else {
        ast_free(node->left);
        ast_free(node->right);
    }
    free(node);
}

void tokens_free(Token *head) {
    while (head) {
        Token *next = head->next;
        free(head->value);
        free(head);
        head = next;
    }
}
