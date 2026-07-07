/* token.h */
#pragma once
#include <stddef.h>

typedef enum {
    TOK_WORD, TOK_PIPE, TOK_REDIR_IN, TOK_REDIR_OUT,
    TOK_REDIR_APPEND, TOK_REDIR_ERR, TOK_REDIR_ERR_APPEND,
    TOK_BACKGROUND, TOK_SEMICOLON, TOK_NEWLINE,
    TOK_AND, TOK_OR, TOK_LPAREN, TOK_RPAREN,
    TOK_HEREDOC,        /* << */
    TOK_HEREDOC_STRIP,  /* <<- (strip leading tabs) */
    TOK_PROCSUB_IN,     /* <( */
    TOK_PROCSUB_OUT,    /* >( */
    TOK_EOF
} TokenType;

typedef struct Token {
    TokenType    type;
    char        *value;       /* pool-allocated string */
    int          quoted;      /* 1 if was quoted */
    int          line, col;   /* source location */
    struct Token *next;
} Token;

void tokens_free(Token *head);
