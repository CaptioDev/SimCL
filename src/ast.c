/*
 * AST implementation for SimCL (Phase 3)
 * Strict ANSI C (C89) compatible.
 */

#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* internal strdup replacement (portable) */
static char *simcl_strdup(const char *s)
{
    char *p;
    size_t len;
    if (!s) return NULL;
    len = strlen(s);
    p = (char*)malloc(len + 1);
    if (!p) return NULL;
    memcpy(p, s, len + 1);
    return p;
}

ASTNode *ast_new_node(ASTNodeType kind, int line)
{
    ASTNode *n = (ASTNode*)malloc(sizeof(ASTNode));
    if (!n) return NULL;
    n->kind = kind;
    n->next = NULL;
    n->line = line;
    n->child = NULL;
    n->name = NULL;
    n->value = NULL;
    n->params = NULL;
    n->op[0] = '\0';
    n->left = NULL;
    n->right = NULL;
    n->literal = NULL;
    return n;
}

ASTNode *ast_new_program(int line)
{
    return ast_new_node(AST_PROGRAM, line);
}

ASTNode *ast_new_block(int line)
{
    return ast_new_node(AST_BLOCK, line);
}

ASTNode *ast_new_let(const char *name, ASTNode *init, int line)
{
    ASTNode *n = ast_new_node(AST_LET, line);
    if (!n) return NULL;
    if (name) n->name = simcl_strdup(name);
    n->value = init;
    return n;
}

ASTNode *ast_new_function(const char *name, ASTNode *params, ASTNode *body, int line)
{
    ASTNode *n = ast_new_node(AST_FUNCTION, line);
    if (!n) return NULL;
    if (name) n->name = simcl_strdup(name);
    n->params = params;
    n->value = body;
    return n;
}

ASTNode *ast_new_return(ASTNode *expr, int line)
{
    ASTNode *n = ast_new_node(AST_RETURN, line);
    if (!n) return NULL;
    n->value = expr;
    return n;
}

ASTNode *ast_new_while(ASTNode *cond, ASTNode *body, int line)
{
    ASTNode *n = ast_new_node(AST_WHILE, line);
    if (!n) return NULL;
    n->value = cond;
    n->child = body;
    return n;
}

ASTNode *ast_new_simulate(ASTNode *body, int line)
{
    ASTNode *n = ast_new_node(AST_SIMULATE, line);
    if (!n) return NULL;
    n->child = body;
    return n;
}

ASTNode *ast_new_expr_stmt(ASTNode *expr, int line)
{
    ASTNode *n = ast_new_node(AST_EXPR_STMT, line);
    if (!n) return NULL;
    n->value = expr;
    return n;
}

ASTNode *ast_new_identifier(const char *name, int line)
{
    ASTNode *n = ast_new_node(AST_IDENTIFIER, line);
    if (!n) return NULL;
    if (name) n->name = simcl_strdup(name);
    return n;
}

ASTNode *ast_new_number(const char *numtext, int line)
{
    ASTNode *n = ast_new_node(AST_NUMBER_LITERAL, line);
    if (!n) return NULL;
    if (numtext) n->literal = simcl_strdup(numtext);
    return n;
}

ASTNode *ast_new_string(const char *text, int line)
{
    ASTNode *n = ast_new_node(AST_STRING_LITERAL, line);
    if (!n) return NULL;
    if (text) n->literal = simcl_strdup(text);
    return n;
}

ASTNode *ast_new_binary(ASTNode *left, const char *op, ASTNode *right, int line)
{
    ASTNode *n = ast_new_node(AST_BINARY_EXPR, line);
    if (!n) return NULL;
    n->left = left;
    if (op) {
        strncpy(n->op, op, sizeof(n->op) - 1);
        n->op[sizeof(n->op)-1] = '\0';
    }
    n->right = right;
    return n;
}

ASTNode *ast_new_unary(const char *op, ASTNode *expr, int line)
{
    ASTNode *n = ast_new_node(AST_UNARY_EXPR, line);
    if (!n) return NULL;
    if (op) {
        strncpy(n->op, op, sizeof(n->op) - 1);
        n->op[sizeof(n->op)-1] = '\0';
    }
    n->value = expr;
    return n;
}

ASTNode *ast_new_call(ASTNode *callee, ASTNode *args, int line)
{
    ASTNode *n = ast_new_node(AST_CALL_EXPR, line);
    if (!n) return NULL;
    n->left = callee;
    n->child = args;
    return n;
}

/* append node to singly-linked list tail pointed by *head */
void ast_list_append(ASTNode **head, ASTNode *node)
{
    ASTNode *cur;
    if (!head || !node) return;
    if (*head == NULL) {
        *head = node;
        node->next = NULL;
        return;
    }
    cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = NULL;
}

/* recursive free */
static void ast_free_recursive(ASTNode *n)
{
    if (!n) return;

    /* free linked list continuation first */
    if (n->next) {
        ast_free_recursive(n->next);
        n->next = NULL;
    }

    /* free children depending on kind */
    if (n->name) {
        free(n->name);
        n->name = NULL;
    }
    if (n->literal) {
        free(n->literal);
        n->literal = NULL;
    }

    if (n->child) {
        ast_free_recursive(n->child);
        n->child = NULL;
    }
    if (n->value) {
        ast_free_recursive(n->value);
        n->value = NULL;
    }
    if (n->params) {
        ast_free_recursive(n->params);
        n->params = NULL;
    }
    if (n->left) {
        ast_free_recursive(n->left);
        n->left = NULL;
    }
    if (n->right) {
        ast_free_recursive(n->right);
        n->right = NULL;
    }

    free(n);
}

void ast_free(ASTNode *node)
{
    ast_free_recursive(node);
}
