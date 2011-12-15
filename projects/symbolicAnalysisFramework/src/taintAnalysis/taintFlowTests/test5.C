#include <stdio.h>

void increment(int *x)
{
    *x = *x + 1;
}

int main()
{
    int x;
    scanf("%d", &x);
    int *p = &x;
    increment(p);
    printf("x=%d\n", x);
    return 0;
}
