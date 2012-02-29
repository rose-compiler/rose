int fib(int n)
{
    int i;
    int a = 0;
    int b = 1;

    for (i = 0; i < n; i++)
    {
        int next = a + b;
        a = b;
        b = next;
    }

    return a;
}

/*
#include <stdio.h>

int main(void)
{
    int i;
    for (i = 0; i < 20; i++)
        printf("fib(%d) = %d\n", i, fib(i));
    return 0;
}
*/
