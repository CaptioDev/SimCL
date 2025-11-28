#ifndef SIMCL_LEXER_H
#define SIMCL_LEXER_H

#include "tokens.h"

typedef struct {
    const char *src;
    int pos;
    int line;
    TokenType type;
    char lexeme[256];
} Lexer;

void lexer_init(Lexer *lex, const char *src);
void lexer_next(Lexer *lex);

#endif
