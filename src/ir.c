#include "ir.h"
#include <stdlib.h>

IRNode *ir_new(IRType t)
{
    IRNode *n = (IRNode*)malloc(sizeof(IRNode));
    if (!n) return 0;
    n->type = t;
    n->next = 0;
    return n;
}

void ir_free(IRNode *n)
{
    free(n);
}
