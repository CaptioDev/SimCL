#ifndef SIMCL_IR_H
#define SIMCL_IR_H

typedef enum {
    IR_NOP = 0
} IRType;

typedef struct IRNode {
    IRType type;
    struct IRNode *next;
} IRNode;

IRNode *ir_new(IRType t);
void ir_free(IRNode *n);

#endif
