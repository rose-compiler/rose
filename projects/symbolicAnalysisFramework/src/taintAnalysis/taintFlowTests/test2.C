#include "test2.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int x = taintFunc1();
    int y = taintFunc2();
    int z = rand()%7;

    
    printf("%d, %d, %d", x, y ,z);
    return 0;
}
