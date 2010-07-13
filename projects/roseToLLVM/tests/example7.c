#include <stdio.h>
#include <stdlib.h>

//
// Test pointer addressing, array & pointer arithmetic
//
int main(int argc, char *argv[]) {
  int a[] = { 1, 2, 3, 4, 5, 0 },
        n = 12,
        k = 1,
        *p = &n,
        *q = &a[3];
 
    printf("n = %i; *p = %i", n, *p);
    printf("; *q = %i\n", *q);

    *q = a[2]; // change the value of a[3] to 2
    printf("q => a[2] = %i\n", *q);

    q = a + 3;
    printf("q => a + 3 = %i\n", *q); // print new value of a[3]

    q = 2 + a;
    printf("q => 2 + a = %i\n", *q); // print value of a[2]

    q = q - k;
    printf("*q => q - 1 = %i\n", *q); // print value of a[1]

    q += 3;
    printf("q += 3 = %i\n", *q); // print value of a[4]

    q--;
    printf("q-- = %i\n", *q); // print value of a[3]

    p = ++q;
    printf("*p <= ++q = %i\n", *p); // print value of a[4]

    for (int *p = a, i = 1; *p; p++,i++) {
        printf("a[%i] = %i\n", i, *p); 
    }

    return 0;
}
