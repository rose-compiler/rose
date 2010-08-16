#include <stdio.h>

int main()
{
    int loc;
    int *p, **q;
    loc = 10;
    p = &loc;
    *p = *p + 1;
    q = &p;
    **q = **q + 1;
    return 0;
}
