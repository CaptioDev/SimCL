#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "ir.h"
#include "optimizer.h"
#include "codegen.h"
#include "vm.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: simcl <file.simcl>\n");
        return 1;
    }
    printf("SimCL Phase 1 Skeleton\n");
    return 0;
}
