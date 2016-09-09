#ifdef __STDC_NO_COMPLEX__
#error "Complex type not supported"
#endif

// workaround is to undefine _COMPLEX_H before
// including <complex.h>
// #undef _COMPLEX_H

#include <complex.h>

typedef complex float complex_float;
typedef float _Complex complex_float;

int main()
{
    complex float x;
    _Complex float y;
    x = 1.0;
    y = 1.0;
    return 0;
}

