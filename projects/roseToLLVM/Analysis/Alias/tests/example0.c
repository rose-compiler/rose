#include <stdio.h>

int main()
{
    int loc;
    int *p;
    loc = 10;
    p = &loc;
    *p = *p + 2;
}
