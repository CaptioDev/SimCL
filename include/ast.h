#ifndef SIMCL_AST_H
#define SIMCL_AST_H

typedef enum {
    AST_NOP = 0
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
} ASTNode;

ASTNode *ast_new(ASTNodeType t);
void ast_free(ASTNode *n);

#endif
