#include <stdio.h>
#include <stdlib.h>

//
// Shift tests
//
int main(int argc, char *argv[]) {
    unsigned b = 1;
    printf("b = 0x%x\n", b);
    for (int i = 0; i < 31; i++) {
        unsigned a =  b << i;
        printf("b << 0x%x = 0x%x\n", i, a);
    }
    b <<= 16;
    printf("b <<= 16 = 0x%x\n", b);


    b = 0xFFFFFFFF;
    printf("b = 0x%x\n", b);
    for (int i = 0; i < 31; i++) {
        unsigned a = b >> i;
        printf("b >> 0x%x = 0x%x\n", i, a);
    }

    b >>= 16;
    printf("b >>= 16 = 0x%x\n", b);

    int c = 1;
    printf("c = %i\n", c);
    for (int i = 0; i < 31; i++) {
        unsigned a =  c << i;
        printf("c << %i = %i\n", i, a);
    }
    c <<= 16;
    printf("c <<= 16 = %i\n", c);


    c = 0xFFFFFFFF;
    printf("c = %i\n", c);
    for (int i = 0; i < 31; i++) {
        unsigned a = c >> i;
        printf("c >> %i = %i\n", i, a);
    }

    b >>= 16;
    printf("b >>= 16 = %i\n", b);

    return 0;
}
