
/*
 * Alias at different scopes for context sensitive results
 */

#include <stdio.h>
void test() {
    int i, j;
    int *p, *q;

    i = 10; j = 20;

    if(i < 20) {
        p = &i;
        *p = *p + 1;
    }
    else {
        q = &j;
        *q = *q + 1;
    }
}
