#ifndef TEST_CODE_BUILDER_INIT_H
#define TEST_CODE_BUILDER_INIT_H

#include <stdlib.h>

template<class T>
void initialize(T& v)
{
    v = rand();
    if (v > RAND_MAX / 2)
        v = 0;
    else
        v -= RAND_MAX / 4;
}

template<>
void initialize(int& i)
{
    i = rand();
    if (i > RAND_MAX / 2)
        i = 0;
    else
        i -= RAND_MAX / 4;
}

#endif
