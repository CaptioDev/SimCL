#ifndef SIMCL_TOKENS_H
#define SIMCL_TOKENS_H

/* Token types for SimCL lexer/parser */
typedef enum {
    TOKEN_EOF = 0,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    /* Keywords */
    TOKEN_LET,
    TOKEN_FUNCTION,
    TOKEN_SIMULATE,
    TOKEN_RETURN,
    TOKEN_WHILE,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_DOUBLE,
    TOKEN_VECTOR,
    TOKEN_MATRIX,

    /* Punctuation / operators */
    TOKEN_LBRACE,   /* { */
    TOKEN_RBRACE,   /* } */
    TOKEN_LPAREN,   /* ( */
    TOKEN_RPAREN,   /* ) */
    TOKEN_COMMA,    /* , */
    TOKEN_SEMI,     /* ; */

    TOKEN_PLUS,     /* + */
    TOKEN_MINUS,    /* - */
    TOKEN_STAR,     /* * */
    TOKEN_SLASH,    /* / */
    TOKEN_PERCENT,  /* % */
    TOKEN_EQUAL,    /* = */
    TOKEN_EQEQ,     /* == */
    TOKEN_NEQ,      /* != */
    TOKEN_LT,       /* < */
    TOKEN_LTE,      /* <= */
    TOKEN_GT,       /* > */
    TOKEN_GTE,      /* >= */

    TOKEN_UNKNOWN
} TokenType;

#endif
