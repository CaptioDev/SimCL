#ifndef SIMCL_AST_H
#define SIMCL_AST_H

/* Abstract Syntax Tree for SimCL - simple, explicit node types
 *
 * Designed for the Phase 3 parser + AST. Memory ownership:
 * - AST nodes are allocated with ast_new_* and freed with ast_free(root).
 * - Strings are duplicated into node fields.
 *
 * Note: simple, explicit C structs to keep compatibility with C89.
 */

#include "tokens.h"

/* Node kinds */
typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_LET,
    AST_FUNCTION,
    AST_RETURN,
    AST_WHILE,
    AST_SIMULATE,
    AST_EXPR_STMT,

    /* Expressions */
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_NUMBER_LITERAL,
    AST_STRING_LITERAL,
    AST_IDENTIFIER,
    AST_CALL_EXPR
} ASTNodeType;

/* Forward */
typedef struct ASTNode ASTNode;

/* Linked list of AST nodes */
struct ASTNode {
    ASTNodeType kind;
    ASTNode *next;   /* for lists of statements / parameters / args */
    int line;        /* source line for diagnostics */

    /* Node-specific fields (union-like manual layout) */

    /* Block / Program: first child is head of statement list */
    ASTNode *child;  /* generic child pointer (e.g., body for functions/block) */

    /* Let: identifier and init expression */
    char *name;      /* identifier name for let, function name, parameter */
    ASTNode *value;  /* initializer or expression / function body pointer */

    /* Function: name in name, child = parameter list (linked identifiers), value = body (block) */
    ASTNode *params; /* linked list of identifier nodes for parameters */

    /* Return: value */
    /* While: condition in value, child = body */
    /* Simulate: child = body */

    /* Expressions: binary/unary */
    char op[4];      /* operator text e.g., "+", "==", "<=" */
    ASTNode *left;
    ASTNode *right;

    /* Literals */
    char *literal;   /* number or string literal text */

    /* Call expression: left = callee (identifier or expr), child = arg list (linked exprs) */
};

/* Constructor helpers */
ASTNode *ast_new_node(ASTNodeType kind, int line);
ASTNode *ast_new_program(int line);
ASTNode *ast_new_block(int line);
ASTNode *ast_new_let(const char *name, ASTNode *init, int line);
ASTNode *ast_new_function(const char *name, ASTNode *params, ASTNode *body, int line);
ASTNode *ast_new_return(ASTNode *expr, int line);
ASTNode *ast_new_while(ASTNode *cond, ASTNode *body, int line);
ASTNode *ast_new_simulate(ASTNode *body, int line);
ASTNode *ast_new_expr_stmt(ASTNode *expr, int line);

ASTNode *ast_new_identifier(const char *name, int line);
ASTNode *ast_new_number(const char *numtext, int line);
ASTNode *ast_new_string(const char *text, int line);
ASTNode *ast_new_binary(ASTNode *left, const char *op, ASTNode *right, int line);
ASTNode *ast_new_unary(const char *op, ASTNode *expr, int line);
ASTNode *ast_new_call(ASTNode *callee, ASTNode *args, int line);

/* list utilities */
void ast_list_append(ASTNode **head, ASTNode *node);

/* Memory */
void ast_free(ASTNode *node);

#endif
