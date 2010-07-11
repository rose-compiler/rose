#include <stdio.h>

//
// Test Comma expressions
//
int main(int argc, char *argv[]) {
    for (int i = 0, k = 0; i < 10; i++, k += 2)
        printf ("i = %i;  k = %i\n", i, k);

    printf("%i\n", (3,2,5));

    int l = (6,7,9);

    printf("%i\n", l);

    return 0;
}
