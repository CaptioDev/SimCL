#ifndef SIMCL_TYPE_SYSTEM_H
#define SIMCL_TYPE_SYSTEM_H

/* Type system for SimCL (Phase 4)
 * ANSI C compatible, simple type representation for semantic analysis
 */

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_VECTOR,
    TYPE_MATRIX,
    TYPE_FUNCTION,
    TYPE_VOID,
    TYPE_UNKNOWN
} SimCLType;

#endif
