#ifndef SIMCL_BYTECODE_H
#define SIMCL_BYTECODE_H

typedef struct {
    unsigned char *data;
    int capacity;
    int length;
} BytecodeBuffer;

void bytecode_init(BytecodeBuffer *b);
void bytecode_free(BytecodeBuffer *b);
void bytecode_emit(BytecodeBuffer *b, unsigned char op);

#endif
