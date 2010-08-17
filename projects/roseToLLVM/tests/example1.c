#include <stdio.h>

//
// Test /= and %= operators
//
int a[] = { 0, 1, 2, 3, 4, 5};

int main(int argc, char *argv[]) {
    int i = 4,
        j = 2,
        k = 3;

    printf("i = %i;  j = %i;\n", i, j);
    i /= j;
    k %= 2;
    printf("i /= j => %i;  k %%= 2 %i;\n", i, k);

    float f = 12.0;
    printf("f = %f;\n", f);
    f /= 2;
    printf("f /= 2 => %f;\n", f);

    return 0;
}
