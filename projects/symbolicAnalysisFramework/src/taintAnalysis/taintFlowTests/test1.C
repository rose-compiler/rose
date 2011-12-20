#include <stdio.h>
#include "test2.h"

int main(int argc, char *argv[])
{
    int i = constTaintFunc1();
    int two = 2;
    int j, l, k;

    if(i % 2 == 0) {
        j = i;
        l = j;
    }
    else {
        k = two * two;
        l = k;
    }

    printf ("%d, %d, %d, %d", i, j, l, k);

    return 0;
}
