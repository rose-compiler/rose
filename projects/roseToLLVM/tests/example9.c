#include <stdio.h>
#include <stdlib.h>

//
// Bit tests
//
int main(int argc, char *argv[]) {
    unsigned a = 0xAAAAAAAA;
    unsigned b = 0x55555555;
    unsigned c;

    c = a ^ b;
    printf("a = 0x%8x; b = 0x%8x; a ^ b = 0x%8x\n", a, b, c);

    c = a & b;
    printf("a = 0x%8x; b = 0x%8x; a & c = 0x%8x\n", a, b, c);

    c = a | b;
    printf("a = 0x%8x; b = 0x%8x; a | c = 0x%8x\n", a, b, c);

    a ^= 0xFF00FF00;
    printf("a ^= 0xFF00FF00 = 0x%8x\n", a);

    b &= 0xFF00FF00;
    printf("b &= 0xFF00FF00 = 0x%8x\n", b);

    c |= 0xFF00FF00;
    printf("c |= 0xFF00FF00 = 0x%8x\n", c);

    return 0;
}
