/*
 * Strict ANSI C (C89) compatible lexer for SimCL
 *
 * Implements:
 *  - identifiers and keywords
 *  - numbers (integer, floating point, scientific notation)
 *  - strings (double-quoted, supports escaped quotes and escapes)
 *  - comments: // line comments and /* block comments *\/
 *  - operators and punctuation
 *
 * The lexer provides the current token in Lexer.type and the lexeme text in
 * Lexer.lexeme (NUL-terminated). Call lexer_next() to advance.
 */

#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* forward declarations for internal helpers */
static char lexer_peek(const Lexer *lex);
static char lexer_peek_next(const Lexer *lex);
static char lexer_advance(Lexer *lex);
static void lexer_set_token(Lexer *lex, TokenType t, const char *text);
static void lexer_clear_lexeme(Lexer *lex);
static void lexer_copy_lexeme_range(Lexer *lex, int start, int end);
static void lexer_skip_whitespace_and_comments(Lexer *lex);
static void lexer_read_number(Lexer *lex);
static void lexer_read_identifier_or_keyword(Lexer *lex);
static void lexer_read_string(Lexer *lex);

/* keyword table */
typedef struct {
    const char *name;
    TokenType type;
} Keyword;

static const Keyword keywords[] = {
    {"let", TOKEN_LET},
    {"function", TOKEN_FUNCTION},
    {"simulate", TOKEN_SIMULATE},
    {"return", TOKEN_RETURN},
    {"while", TOKEN_WHILE},
    {"int", TOKEN_INT},
    {"float", TOKEN_FLOAT},
    {"double", TOKEN_DOUBLE},
    {"vector", TOKEN_VECTOR},
    {"matrix", TOKEN_MATRIX},
    {NULL, TOKEN_UNKNOWN}
};

void lexer_init(Lexer *lex, const char *src)
{
    if (!lex) return;
    lex->src = src ? src : "";
    lex->pos = 0;
    lex->line = 1;
    lex->type = TOKEN_EOF;
    lexer_clear_lexeme(lex);
}

static char lexer_peek(const Lexer *lex)
{
    char c = lex->src[lex->pos];
    return c ? c : '\0';
}

static char lexer_peek_next(const Lexer *lex)
{
    char c = lex->src[lex->pos + 1];
    return c ? c : '\0';
}

static char lexer_advance(Lexer *lex)
{
    char c = lex->src[lex->pos];
    if (c == '\0') return '\0';
    lex->pos++;
    if (c == '\n') lex->line++;
    return c;
}

static void lexer_set_token(Lexer *lex, TokenType t, const char *text)
{
    lex->type = t;
    if (text) {
        /* copy up to SIMCL_LEXEME_MAX-1 characters */
        int i;
        for (i = 0; i < SIMCL_LEXEME_MAX - 1 && text[i]; ++i) {
            lex->lexeme[i] = text[i];
        }
        lex->lexeme[i] = '\0';
    } else {
        lex->lexeme[0] = '\0';
    }
}

static void lexer_clear_lexeme(Lexer *lex)
{
    lex->lexeme[0] = '\0';
}

/* copy substring from start (inclusive) to end (exclusive) into lexeme */
static void lexer_copy_lexeme_range(Lexer *lex, int start, int end)
{
    int i = 0;
    int j;
    for (j = start; j < end && i < SIMCL_LEXEME_MAX - 1; ++j) {
        lex->lexeme[i++] = lex->src[j];
    }
    lex->lexeme[i] = '\0';
}

/* Skip whitespace and comments. Advances lex->pos to first non-whitespace/comment */
static void lexer_skip_whitespace_and_comments(Lexer *lex)
{
    for (;;) {
        char c = lexer_peek(lex);
        /* whitespace */
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            lexer_advance(lex);
            continue;
        }

        /* line comment: // ... */
        if (c == '/' && lexer_peek_next(lex) == '/') {
            /* consume // */
            lexer_advance(lex);
            lexer_advance(lex);
            /* skip until newline or NUL */
            while (lexer_peek(lex) != '\0' && lexer_peek(lex) != '\n') {
                lexer_advance(lex);
            }
            continue;
        }

        /* block comment: /* ... *\/ */
        if (c == '/' && lexer_peek_next(lex) == '*') {
            /* consume /* */
            lexer_advance(lex);
            lexer_advance(lex);
            /* scan until closing */ */
            while (lexer_peek(lex) != '\0') {
                if (lexer_peek(lex) == '*' && lexer_peek_next(lex) == '/') {
                    lexer_advance(lex); /* '*' */
                    lexer_advance(lex); /* '/' */
                    break;
                } else {
                    lexer_advance(lex);
                }
            }
            continue;
        }

        break;
    }
}

/* Read a number starting at lex->pos. Supports integers, floats, exponentials.
   After returning, lex->pos points just after the last character of the number.
*/
static void lexer_read_number(Lexer *lex)
{
    int start = lex->pos;
    int saw_dot = 0;
    int saw_exp = 0;

    /* integer part */
    while (isdigit((unsigned char)lexer_peek(lex))) {
        lexer_advance(lex);
    }

    /* fractional part */
    if (lexer_peek(lex) == '.') {
        saw_dot = 1;
        lexer_advance(lex);
        while (isdigit((unsigned char)lexer_peek(lex))) {
            lexer_advance(lex);
        }
    }

    /* exponent part */
    if (lexer_peek(lex) == 'e' || lexer_peek(lex) == 'E') {
        saw_exp = 1;
        lexer_advance(lex);
        /* optional sign */
        if (lexer_peek(lex) == '+' || lexer_peek(lex) == '-') {
            lexer_advance(lex);
        }
        /* digits of exponent */
        while (isdigit((unsigned char)lexer_peek(lex))) {
            lexer_advance(lex);
        }
    }

    /* copy lexeme range */
    lexer_copy_lexeme_range(lex, start, lex->pos);
    lexer_set_token(lex, TOKEN_NUMBER, lex->lexeme);
}

/* Read identifier or keyword */
static void lexer_read_identifier_or_keyword(Lexer *lex)
{
    int start = lex->pos;
    lexer_advance(lex); /* first char already known to be alpha/_ */
    while (isalnum((unsigned char)lexer_peek(lex)) || lexer_peek(lex) == '_') {
        lexer_advance(lex);
    }
    lexer_copy_lexeme_range(lex, start, lex->pos);

    /* check keywords */
    {
        const Keyword *k = keywords;
        while (k->name) {
            if (strcmp(k->name, lex->lexeme) == 0) {
                lexer_set_token(lex, k->type, lex->lexeme);
                return;
            }
            ++k;
        }
    }

    lexer_set_token(lex, TOKEN_IDENTIFIER, lex->lexeme);
}

/* Read double-quoted string. Supports simple escape sequences: \", \\ , \n, \t */
static void lexer_read_string(Lexer *lex)
{
    int start = lex->pos; /* points at opening quote */
    lexer_advance(lex);   /* consume opening " */

    /* build into a temporary buffer because we process escapes */
    char buf[SIMCL_LEXEME_MAX];
    int bi = 0;

    while (lexer_peek(lex) != '\0') {
        char c = lexer_advance(lex);
        if (c == '"') {
            /* end of string */
            break;
        }
        if (c == '\\') {
            /* escape */
            char n = lexer_peek(lex);
            if (n == '\0') break;
            n = lexer_advance(lex);
            if (n == 'n') {
                if (bi < SIMCL_LEXEME_MAX - 1) buf[bi++] = '\n';
            } else if (n == 't') {
                if (bi < SIMCL_LEXEME_MAX - 1) buf[bi++] = '\t';
            } else if (n == '"') {
                if (bi < SIMCL_LEXEME_MAX - 1) buf[bi++] = '"';
            } else if (n == '\\') {
                if (bi < SIMCL_LEXEME_MAX - 1) buf[bi++] = '\\';
            } else {
                /* unknown escape: copy literally */
                if (bi < SIMCL_LEXEME_MAX - 1) buf[bi++] = n;
            }
        } else {
            if (bi < SIMCL_LEXEME_MAX - 1) buf[bi++] = c;
        }
    }

    buf[bi] = '\0';
    lexer_set_token(lex, TOKEN_STRING, buf);
}

/* Public: advance lexer to next token */
void lexer_next(Lexer *lex)
{
    if (!lex) return;

    lexer_clear_lexeme(lex);
    lexer_skip_whitespace_and_comments(lex);

    char c = lexer_peek(lex);

    if (c == '\0') {
        lexer_set_token(lex, TOKEN_EOF, NULL);
        return;
    }

    /* identifiers or keywords */
    if (isalpha((unsigned char)c) || c == '_') {
        lexer_read_identifier_or_keyword(lex);
        return;
    }

    /* numbers */
    if (isdigit((unsigned char)c) || (c == '.' && isdigit((unsigned char)lexer_peek_next(lex)))) {
        lexer_read_number(lex);
        return;
    }

    /* strings */
    if (c == '"') {
        lexer_read_string(lex);
        return;
    }

    /* operators and punctuation */
    /* two-character operators first */
    if (c == '=') {
        if (lexer_peek_next(lex) == '=') {
            lexer_advance(lex);
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_EQEQ, "==");
            return;
        } else {
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_EQUAL, "=");
            return;
        }
    }

    if (c == '!' && lexer_peek_next(lex) == '=') {
        lexer_advance(lex);
        lexer_advance(lex);
        lexer_set_token(lex, TOKEN_NEQ, "!=");
        return;
    }

    if (c == '<') {
        if (lexer_peek_next(lex) == '=') {
            lexer_advance(lex);
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_LTE, "<=");
            return;
        } else {
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_LT, "<");
            return;
        }
    }

    if (c == '>') {
        if (lexer_peek_next(lex) == '=') {
            lexer_advance(lex);
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_GTE, ">=");
            return;
        } else {
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_GT, ">");
            return;
        }
    }

    /* single-character tokens */
    switch (c) {
    case '+': lexer_advance(lex); lexer_set_token(lex, TOKEN_PLUS, "+"); return;
    case '-': lexer_advance(lex); lexer_set_token(lex, TOKEN_MINUS, "-"); return;
    case '*': lexer_advance(lex); lexer_set_token(lex, TOKEN_STAR, "*"); return;
    case '/': lexer_advance(lex); lexer_set_token(lex, TOKEN_SLASH, "/"); return;
    case '%': lexer_advance(lex); lexer_set_token(lex, TOKEN_PERCENT, "%"); return;
    case '{': lexer_advance(lex); lexer_set_token(lex, TOKEN_LBRACE, "{"); return;
    case '}': lexer_advance(lex); lexer_set_token(lex, TOKEN_RBRACE, "}"); return;
    case '(': lexer_advance(lex); lexer_set_token(lex, TOKEN_LPAREN, "("); return;
    case ')': lexer_advance(lex); lexer_set_token(lex, TOKEN_RPAREN, ")"); return;
    case ',': lexer_advance(lex); lexer_set_token(lex, TOKEN_COMMA, ","); return;
    case ';': lexer_advance(lex); lexer_set_token(lex, TOKEN_SEMI, ";"); return;
    default:
        /* unknown / single char fallback */
        {
            char tmp[2];
            tmp[0] = c;
            tmp[1] = '\0';
            lexer_advance(lex);
            lexer_set_token(lex, TOKEN_UNKNOWN, tmp);
            return;
        }
    }
}
