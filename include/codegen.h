#ifndef SIMCL_CODEGEN_H
#define SIMCL_CODEGEN_H

#include "ir.h"
#include "bytecode.h"

void codegen_emit(IRNode *ir, BytecodeBuffer *buf);

#endif
