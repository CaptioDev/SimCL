#ifndef SIMCL_LEXER_H
#define SIMCL_LEXER_H

#include "tokens.h"

/* Maximum single lexeme length (including terminating NUL) */
#define SIMCL_LEXEME_MAX 256

typedef struct {
    const char *src;               /* input source (NUL terminated) */
    int pos;                       /* current index into src */
    int line;                      /* current line number (1-based) */
    TokenType type;                /* current token type */
    char lexeme[SIMCL_LEXEME_MAX]; /* current token text */
} Lexer;

/* Initialize lexer with source string */
void lexer_init(Lexer *lex, const char *src);

/* Advance to next token (fills lex->type and lex->lexeme) */
void lexer_next(Lexer *lex);

#endif
