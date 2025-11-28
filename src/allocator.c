#include "allocator.h"
#include <stdlib.h>

void *simcl_malloc(long size)
{
    return malloc(size);
}

void simcl_free(void *p)
{
    free(p);
}
