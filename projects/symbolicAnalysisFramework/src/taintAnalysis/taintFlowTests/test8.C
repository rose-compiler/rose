#include <stdio.h>
#include "test2.h"

int main()
{
    int x = constTaintFunc1();
    int y = constTaintFunc2();

    for( ; x + y <= 12;) {
        x++;
        y++;
    }

    printf ("%d, %d\n", x, y);

    return 0;
}
