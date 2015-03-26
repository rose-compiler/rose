#include <cstdio>

int f()
{
    int c[2] = {1,2};
    for (int i = 0; i < 2; i++)
        c[i] = i;
    for (int i = 0; i < 2; i++)
        printf("%d ", c[i]);
} 
