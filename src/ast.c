#include "ast.h"
#include <stdlib.h>

ASTNode *ast_new(ASTNodeType t)
{
    ASTNode *n = (ASTNode*)malloc(sizeof(ASTNode));
    if (!n) return 0;
    n->type = t;
    return n;
}

void ast_free(ASTNode *n)
{
    free(n);
}
