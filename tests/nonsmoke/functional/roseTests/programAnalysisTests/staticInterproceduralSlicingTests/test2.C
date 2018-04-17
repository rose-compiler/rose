#include<stdio.h>

int add(int a, int b);

void main() {
    int i = 1;     int sum = 0;
    while (i<11) {
        sum = add(sum, i);
        i = add(i, 1);
    }
    printf("sum = %d\n", sum);

#pragma SliceTarget
    i;
    printf("i = %d\n", i);
}

static int add(int a, int b)
{
    return(a+b);
}
