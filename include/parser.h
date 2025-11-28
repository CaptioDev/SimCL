#ifndef SIMCL_PARSER_H
#define SIMCL_PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    Lexer *lex;
} Parser;

void parser_init(Parser *p, Lexer *lex);
ASTNode *parser_parse(Parser *p);

#endif
