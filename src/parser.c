/*
 * Recursive-descent parser for SimCL (Phase 3)
 *
 * Grammar (approximate):
 *
 * program       := { statement }
 * statement     := let_stmt
 *                | function_decl
 *                | simulate_block
 *                | return_stmt
 *                | while_stmt
 *                | expr_stmt
 * let_stmt      := "let" identifier "=" expression [";"]
 * function_decl := "function" identifier "(" [param_list] ")" block
 * simulate_block:= "simulate" block
 * return_stmt   := "return" expression [";"]
 * while_stmt    := "while" expression block
 * block         := "{" { statement } "}"
 * expr_stmt     := expression [";"]
 *
 * expression    := assignment
 * assignment    := equality [ "=" assignment ]
 * equality      := relational { ("==" | "!=") relational }
 * relational    := additive { ("<" | "<=" | ">" | ">=") additive }
 * additive      := multiplicative { ("+" | "-") multiplicative }
 * multiplicative:= unary { ("*" | "/" | "%") unary }
 * unary         := ("+" | "-" | "!") unary | primary
 * primary       := number | string | identifier [ "(" [arg_list] ")" ] | "(" expression ")"
 *
 * Note: This parser builds an AST using ast_new_* helpers.
 *
 * Strict ANSI C (C89) implementation.
 */

#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* internal helper prototypes */
static void parser_error(Parser *p, const char *fmt, ...);
static void advance(Parser *p);
static int accept(Parser *p, TokenType t);
static void expect(Parser *p, TokenType t);

static ASTNode *parse_statement(Parser *p);
static ASTNode *parse_block(Parser *p);
static ASTNode *parse_let(Parser *p);
static ASTNode *parse_function(Parser *p);
static ASTNode *parse_simulate(Parser *p);
static ASTNode *parse_return(Parser *p);
static ASTNode *parse_while(Parser *p);
static ASTNode *parse_expression(Parser *p);
static ASTNode *parse_assignment(Parser *p);
static ASTNode *parse_equality(Parser *p);
static ASTNode *parse_relational(Parser *p);
static ASTNode *parse_additive(Parser *p);
static ASTNode *parse_multiplicative(Parser *p);
static ASTNode *parse_unary(Parser *p);
static ASTNode *parse_primary(Parser *p);
static ASTNode *parse_arg_list(Parser *p);
static ASTNode *parse_param_list(Parser *p);

/* convenience reference to current token */
#define CURTOK (p->lex->type)
#define CURLEX (p->lex->lexeme)
#define CURLINE (p->lex->line)

void parser_init(Parser *p, Lexer *lex)
{
    p->lex = lex;
    /* prime lexer to first token */
    lexer_next(p->lex);
}

static void parser_error(Parser *p, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Parser error (line %d): ", CURLINE);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    /* In this phase we exit on error to simplify control flow */
    exit(1);
}

static void advance(Parser *p)
{
    lexer_next(p->lex);
}

static int accept(Parser *p, TokenType t)
{
    if (CURTOK == t) {
        advance(p);
        return 1;
    }
    return 0;
}

static void expect(Parser *p, TokenType t)
{
    if (CURTOK != t) {
        parser_error(p, "expected token %d but got %d ('%s')", (int)t, (int)CURTOK, CURLEX);
    }
    advance(p);
}

/* Top-level parse */
ASTNode *parser_parse(Parser *p)
{
    ASTNode *root = ast_new_program(CURLINE);
    ASTNode *last = NULL;

    while (CURTOK != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(p);
        if (stmt) {
            if (root->child == NULL) {
                root->child = stmt;
            } else {
                ast_list_append(&root->child, stmt);
            }
        } else {
            /* skip token to avoid infinite loop */
            advance(p);
        }
    }
    return root;
}

/* statement parsing */
static ASTNode *parse_statement(Parser *p)
{
    if (CURTOK == TOKEN_LET) {
        return parse_let(p);
    }
    if (CURTOK == TOKEN_FUNCTION) {
        return parse_function(p);
    }
    if (CURTOK == TOKEN_SIMULATE) {
        return parse_simulate(p);
    }
    if (CURTOK == TOKEN_RETURN) {
        return parse_return(p);
    }
    if (CURTOK == TOKEN_WHILE) {
        return parse_while(p);
    }

    /* default: expression statement */
    {
        ASTNode *expr = parse_expression(p);
        if (accept(p, TOKEN_SEMI)) {
            /* swallow optional semicolon */
        }
        if (expr) {
            return ast_new_expr_stmt(expr, CURLINE);
        }
        return NULL;
    }
}

static ASTNode *parse_block(Parser *p)
{
    expect(p, TOKEN_LBRACE);
    ASTNode *block = ast_new_block(CURLINE);
    while (CURTOK != TOKEN_RBRACE && CURTOK != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(p);
        if (stmt) {
            ast_list_append(&block->child, stmt);
        } else {
            /* skip unexpected token to avoid infinite loop */
            advance(p);
        }
    }
    expect(p, TOKEN_RBRACE);
    return block;
}

static ASTNode *parse_let(Parser *p)
{
    /* let identifier = expr ; */
    expect(p, TOKEN_LET);
    if (CURTOK != TOKEN_IDENTIFIER) {
        parser_error(p, "expected identifier after 'let'");
    }
    {
        char namebuf[256];
        strncpy(namebuf, CURLEX, sizeof(namebuf)-1);
        namebuf[sizeof(namebuf)-1] = '\0';
        advance(p);
        expect(p, TOKEN_EQUAL);
        ASTNode *expr = parse_expression(p);
        accept(p, TOKEN_SEMI);
        return ast_new_let(namebuf, expr, CURLINE);
    }
}

static ASTNode *parse_function(Parser *p)
{
    expect(p, TOKEN_FUNCTION);
    if (CURTOK != TOKEN_IDENTIFIER) {
        parser_error(p, "expected function name");
    }
    {
        char namebuf[256];
        strncpy(namebuf, CURLEX, sizeof(namebuf)-1);
        namebuf[sizeof(namebuf)-1] = '\0';
        advance(p);
        expect(p, TOKEN_LPAREN);
        ASTNode *params = NULL;
        if (CURTOK != TOKEN_RPAREN) {
            params = parse_param_list(p);
        }
        expect(p, TOKEN_RPAREN);
        ASTNode *body = parse_block(p);
        return ast_new_function(namebuf, params, body, CURLINE);
    }
}

static ASTNode *parse_simulate(Parser *p)
{
    expect(p, TOKEN_SIMULATE);
    ASTNode *body = parse_block(p);
    return ast_new_simulate(body, CURLINE);
}

static ASTNode *parse_return(Parser *p)
{
    expect(p, TOKEN_RETURN);
    ASTNode *expr = parse_expression(p);
    accept(p, TOKEN_SEMI);
    return ast_new_return(expr, CURLINE);
}

static ASTNode *parse_while(Parser *p)
{
    expect(p, TOKEN_WHILE);
    ASTNode *cond = parse_expression(p);
    ASTNode *body = parse_block(p);
    return ast_new_while(cond, body, CURLINE);
}

/* expression parsing (precedence climbing via recursive descent) */
static ASTNode *parse_expression(Parser *p)
{
    return parse_assignment(p);
}

static ASTNode *parse_assignment(Parser *p)
{
    ASTNode *left = parse_equality(p);
    if (CURTOK == TOKEN_EQUAL) {
        /* assignment - right-associative */
        /* Only allow identifier on left for simple language */
        if (!left || left->kind != AST_IDENTIFIER) {
            parser_error(p, "invalid assignment target");
        }
        expect(p, TOKEN_EQUAL);
        ASTNode *right = parse_assignment(p);
        /* represent assignment as binary node with op "=" */
        return ast_new_binary(left, "=", right, CURLINE);
    }
    return left;
}

static ASTNode *parse_equality(Parser *p)
{
    ASTNode *node = parse_relational(p);
    while (CURTOK == TOKEN_EQEQ || CURTOK == TOKEN_NEQ) {
        char opbuf[4];
        if (CURTOK == TOKEN_EQEQ) strncpy(opbuf, "==", sizeof(opbuf)-1);
        else strncpy(opbuf, "!=", sizeof(opbuf)-1);
        opbuf[sizeof(opbuf)-1] = '\0';
        advance(p);
        ASTNode *rhs = parse_relational(p);
        node = ast_new_binary(node, opbuf, rhs, CURLINE);
    }
    return node;
}

static ASTNode *parse_relational(Parser *p)
{
    ASTNode *node = parse_additive(p);
    while (CURTOK == TOKEN_LT || CURTOK == TOKEN_LTE || CURTOK == TOKEN_GT || CURTOK == TOKEN_GTE) {
        char opbuf[4];
        if (CURTOK == TOKEN_LT) strncpy(opbuf, "<", sizeof(opbuf)-1);
        else if (CURTOK == TOKEN_LTE) strncpy(opbuf, "<=", sizeof(opbuf)-1);
        else if (CURTOK == TOKEN_GT) strncpy(opbuf, ">", sizeof(opbuf)-1);
        else strncpy(opbuf, ">=", sizeof(opbuf)-1);
        opbuf[sizeof(opbuf)-1] = '\0';
        advance(p);
        ASTNode *rhs = parse_additive(p);
        node = ast_new_binary(node, opbuf, rhs, CURLINE);
    }
    return node;
}

static ASTNode *parse_additive(Parser *p)
{
    ASTNode *node = parse_multiplicative(p);
    while (CURTOK == TOKEN_PLUS || CURTOK == TOKEN_MINUS) {
        char opbuf[4];
        if (CURTOK == TOKEN_PLUS) strncpy(opbuf, "+", sizeof(opbuf)-1);
        else strncpy(opbuf, "-", sizeof(opbuf)-1);
        opbuf[sizeof(opbuf)-1] = '\0';
        advance(p);
        ASTNode *rhs = parse_multiplicative(p);
        node = ast_new_binary(node, opbuf, rhs, CURLINE);
    }
    return node;
}

static ASTNode *parse_multiplicative(Parser *p)
{
    ASTNode *node = parse_unary(p);
    while (CURTOK == TOKEN_STAR || CURTOK == TOKEN_SLASH || CURTOK == TOKEN_PERCENT) {
        char opbuf[4];
        if (CURTOK == TOKEN_STAR) strncpy(opbuf, "*", sizeof(opbuf)-1);
        else if (CURTOK == TOKEN_SLASH) strncpy(opbuf, "/", sizeof(opbuf)-1);
        else strncpy(opbuf, "%", sizeof(opbuf)-1);
        opbuf[sizeof(opbuf)-1] = '\0';
        advance(p);
        ASTNode *rhs = parse_unary(p);
        node = ast_new_binary(node, opbuf, rhs, CURLINE);
    }
    return node;
}

static ASTNode *parse_unary(Parser *p)
{
    if (CURTOK == TOKEN_PLUS || CURTOK == TOKEN_MINUS) {
        char opbuf[4];
        if (CURTOK == TOKEN_PLUS) strncpy(opbuf, "+", sizeof(opbuf)-1);
        else strncpy(opbuf, "-", sizeof(opbuf)-1);
        opbuf[sizeof(opbuf)-1] = '\0';
        advance(p);
        ASTNode *expr = parse_unary(p);
        return ast_new_unary(opbuf, expr, CURLINE);
    }
    return parse_primary(p);
}

static ASTNode *parse_primary(Parser *p)
{
    if (CURTOK == TOKEN_NUMBER) {
        ASTNode *n = ast_new_number(CURLEX, CURLINE);
        advance(p);
        return n;
    }
    if (CURTOK == TOKEN_STRING) {
        ASTNode *n = ast_new_string(CURLEX, CURLINE);
        advance(p);
        return n;
    }
    if (CURTOK == TOKEN_IDENTIFIER) {
        /* identifier or call */
        ASTNode *id = ast_new_identifier(CURLEX, CURLINE);
        advance(p);
        if (CURTOK == TOKEN_LPAREN) {
            /* call */
            advance(p); /* consume '(' */
            ASTNode *args = NULL;
            if (CURTOK != TOKEN_RPAREN) {
                args = parse_arg_list(p);
            }
            expect(p, TOKEN_RPAREN);
            return ast_new_call(id, args, CURLINE);
        }
        return id;
    }

    if (CURTOK == TOKEN_LPAREN) {
        advance(p);
        ASTNode *expr = parse_expression(p);
        expect(p, TOKEN_RPAREN);
        return expr;
    }

    /* unexpected token */
    parser_error(p, "unexpected token '%s' in primary expression", CURLEX);
    return NULL;
}

/* parse comma-separated arg list (expressions) */
static ASTNode *parse_arg_list(Parser *p)
{
    ASTNode *head = NULL;
    while (CURTOK != TOKEN_RPAREN && CURTOK != TOKEN_EOF) {
        ASTNode *expr = parse_expression(p);
        ast_list_append(&head, expr);
        if (CURTOK == TOKEN_COMMA) {
            advance(p);
            continue;
        }
        break;
    }
    return head;
}

/* parse comma-separated parameter list (identifiers) */
static ASTNode *parse_param_list(Parser *p)
{
    ASTNode *head = NULL;
    while (CURTOK != TOKEN_RPAREN && CURTOK != TOKEN_EOF) {
        if (CURTOK != TOKEN_IDENTIFIER) {
            parser_error(p, "expected parameter name");
        }
        ASTNode *id = ast_new_identifier(CURLEX, CURLINE);
        advance(p);
        ast_list_append(&head, id);
        if (CURTOK == TOKEN_COMMA) {
            advance(p);
            continue;
        }
        break;
    }
    return head;
}
