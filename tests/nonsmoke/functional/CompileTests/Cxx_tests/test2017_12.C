#include <stdio.h>
#include <cfloat>

// DQ (3/30/2017): We now include this as defined in ROSE
// (specifically in rose_edg_required_macros_and_functions.h).
// typedef long double __float128;

typedef __float128  long_double_t;

int main()
{
#if 0
#ifdef  USE_FLOAT128
    typedef __float128  long_double_t;
#else
// #error "USE_FLOAT128 is not defined!"
    typedef long double long_double_t;
#endif
#endif

long_double_t ld;

int* i = (int*) &ld;
i[0] = i[1] = i[2] = i[3] = 0xdeadbeef;

for(ld = 0.0000000000000001; ld < LDBL_MAX; ld *= 1.0000001)
    printf("%08x-%08x-%08x-%08x\r", i[0], i[1], i[2], i[3]);

return 0;
}

