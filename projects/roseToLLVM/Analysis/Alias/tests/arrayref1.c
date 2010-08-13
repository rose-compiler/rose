#include <stdio.h>

int main()
{
    // array ref alias test
    int a[10];
    int *p;
    int i;
    for(i = 0; i < 10; i++)
        a[i] = i;

    p = a + 5;
    *p = a[5] + 1;
    return 0;
}        
