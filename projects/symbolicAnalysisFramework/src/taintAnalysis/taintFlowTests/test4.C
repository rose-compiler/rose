#include <stdio.h>

void incrementref(int &x)
{
    x++;
}

int main()
{
    int x;
    x = 10;
    scanf("%d", &x);
    incrementref(x);
    printf("x = %d", x);
    return 0;
}
