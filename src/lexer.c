/* lexer.c — Tokenizer */
#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

Token *lexer_tokenize(const char *input) {
    Token  *head = NULL, *tail = NULL;
    const char *p = input;
    char    buf[4096];
    int     i;

    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;

        Token *tok = calloc(1, sizeof(Token));

        if (p[0]=='&' && p[1]=='&') { tok->type=TOKEN_AND;        p+=2; }
        else if (p[0]=='|' && p[1]=='|') { tok->type=TOKEN_OR;    p+=2; }
        else if (p[0]=='>' && p[1]=='>') { tok->type=TOKEN_REDIR_APPEND; p+=2; }
        else if (p[0]=='2' && p[1]=='>') { tok->type=TOKEN_REDIR_ERR; p+=2; }
        else if (*p == '|') { tok->type=TOKEN_PIPE;       p++; }
        else if (*p == '<') { tok->type=TOKEN_REDIR_IN;   p++; }
        else if (*p == '>') { tok->type=TOKEN_REDIR_OUT;  p++; }
        else if (*p == '&') { tok->type=TOKEN_BACKGROUND; p++; }
        else if (*p == ';') { tok->type=TOKEN_SEMICOLON;  p++; }
        else if (*p == '(') { tok->type=TOKEN_LPAREN;     p++; }
        else if (*p == ')') { tok->type=TOKEN_RPAREN;     p++; }
        else {
            i = 0;
            while (*p && !strchr(" \t|<>&;()\n", *p)) {
                if (*p == '\"') {
                    p++;
                    while (*p && *p != '\"') buf[i++] = *p++;
                    if (*p) p++;
                } else if (*p == '\'') {
                    p++;
                    while (*p && *p != '\'') buf[i++] = *p++;
                    if (*p) p++;
                } else if (*p == '\\') {
                    p++; if (*p) buf[i++] = *p++;
                } else {
                    buf[i++] = *p++;
                }
            }
            buf[i] = '\0';
            tok->type  = TOKEN_WORD;
            tok->value = strdup(buf);
        }

        if (!head) head = tail = tok;
        else { tail->next = tok; tail = tok; }
    }
    return head;
}
