#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"

ASTNode *parser_parse(Token *tokens);

#endif
