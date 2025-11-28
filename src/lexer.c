#include "lexer.h"

void lexer_init(Lexer *lex, const char *src)
{
    lex->src = src;
    lex->pos = 0;
    lex->line = 1;
    lex->type = TOKEN_EOF;
    lex->lexeme[0] = '\0';
}

void lexer_next(Lexer *lex)
{
    lex->type = TOKEN_EOF;
}
