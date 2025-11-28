/*
 * Semantic analysis for SimCL
 * Constructs symbol tables and performs basic type checking
 */

#include "semantic.h"
#include "type_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* init semantic context */
void semantic_init(SemanticContext *ctx)
{
    ctx->globals = symtab_new(NULL);
    ctx->current_scope = ctx->globals;
}

/* free context */
void semantic_free(SemanticContext *ctx)
{
    if (ctx->globals) {
        symtab_free(ctx->globals);
        ctx->globals = NULL;
        ctx->current_scope = NULL;
    }
}

/* forward declaration */
static void analyze_node(SemanticContext *ctx, ASTNode *node);

/* analyze list of statements */
static void analyze_list(SemanticContext *ctx, ASTNode *head)
{
    ASTNode *cur = head;
    while (cur) {
        analyze_node(ctx, cur);
        cur = cur->next;
    }
}

static void analyze_node(SemanticContext *ctx, ASTNode *node)
{
    if (!node) return;

    switch (node->kind) {
    case AST_PROGRAM:
    case AST_BLOCK:
        /* new scope for block */
        {
            SymbolTable *prev = ctx->current_scope;
            ctx->current_scope = symtab_new(prev);
            analyze_list(ctx, node->child);
            symtab_free(ctx->current_scope);
            ctx->current_scope = prev;
        }
        break;
    case AST_LET:
        /* infer type: for now default to unknown */
        symtab_add(ctx->current_scope, node->name, TYPE_UNKNOWN);
        analyze_node(ctx, node->value);
        break;
    case AST_FUNCTION:
        symtab_add(ctx->current_scope, node->name, TYPE_FUNCTION);
        {
            SymbolTable *prev = ctx->current_scope;
            ctx->current_scope = symtab_new(prev);
            /* add parameters */
            ASTNode *param = node->params;
            while (param) {
                symtab_add(ctx->current_scope, param->name, TYPE_UNKNOWN);
                param = param->next;
            }
            analyze_node(ctx, node->value); /* body */
            symtab_free(ctx->current_scope);
            ctx->current_scope = prev;
        }
        break;
    case AST_RETURN:
        analyze_node(ctx, node->value);
        break;
    case AST_WHILE:
    case AST_SIMULATE:
        analyze_node(ctx, node->value);
        analyze_node(ctx, node->child);
        break;
    case AST_EXPR_STMT:
        analyze_node(ctx, node->value);
        break;
    case AST_BINARY_EXPR:
    case AST_UNARY_EXPR:
        analyze_node(ctx, node->left);
        analyze_node(ctx, node->right);
        analyze_node(ctx, node->value);
        break;
    case AST_CALL_EXPR:
        analyze_node(ctx, node->left);  /* callee */
        analyze_node(ctx, node->child); /* args */
        break;
    case AST_IDENTIFIER:
    case AST_NUMBER_LITERAL:
    case AST_STRING_LITERAL:
        /* nothing */
        break;
    default:
        fprintf(stderr, "Semantic: unhandled AST node kind %d\n", node->kind);
        break;
    }
}

/* entry point */
void semantic_analyze(SemanticContext *ctx, ASTNode *root)
{
    analyze_node(ctx, root);
}
