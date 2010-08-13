#include <stdio.h>

int main()
{
    // array ref alias test
    int a[10];
    int *p;
    int i, j;
    for(i = 0; i < 10; i++)
        a[i] = i;

    p = a + 5;
    *p = *p + 1;

    for(i = 0; i < 10; i++)
        a[i] = i;


    return 0;
}        
