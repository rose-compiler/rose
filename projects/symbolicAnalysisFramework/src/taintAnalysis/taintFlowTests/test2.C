#include "test2.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int x = constTaintFunc1();
    int y = constTaintFunc2();
    int z = rand()%7;

    
    printf("%d, %d, %d", x, y ,z);
    return 0;
}
