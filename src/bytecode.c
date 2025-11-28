#include "bytecode.h"
#include <stdlib.h>

void bytecode_init(BytecodeBuffer *b)
{
    b->capacity = 128;
    b->length = 0;
    b->data = (unsigned char*)malloc(b->capacity);
}

void bytecode_free(BytecodeBuffer *b)
{
    free(b->data);
}

void bytecode_emit(BytecodeBuffer *b, unsigned char op)
{
    if (b->length >= b->capacity) {
        b->capacity *= 2;
        b->data = (unsigned char*)realloc(b->data, b->capacity);
    }
    b->data[b->length++] = op;
}
