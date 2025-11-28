#ifndef SIMCL_SEMANTIC_H
#define SIMCL_SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

/* Semantic analysis: type checking, symbol table construction */
typedef struct SemanticContext {
    SymbolTable *globals;
    SymbolTable *current_scope;
} SemanticContext;

void semantic_init(SemanticContext *ctx);
void semantic_analyze(SemanticContext *ctx, ASTNode *root);
void semantic_free(SemanticContext *ctx);

#endif
