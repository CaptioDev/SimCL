#include "std_array.h"
#include <stdlib.h>

void *std_array_new(int count)
{
    return malloc(count * sizeof(double));
}
