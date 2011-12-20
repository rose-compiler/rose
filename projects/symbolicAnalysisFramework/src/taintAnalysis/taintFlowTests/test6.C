#include <stdio.h>

int main()
{
    int x;
    int *p;
    x = 10;
    p = &x;
    printf ("%d = %d",x, *p);
    return 0;
}
