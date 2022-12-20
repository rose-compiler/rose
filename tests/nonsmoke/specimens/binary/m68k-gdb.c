// m68k-linux-gnu-gcc -m5200 -nostdlib -Wall -g -O0 -o m68k-gdb m68k-gdb.c
int _start() {
    asm("move.l #0x80000100, %fp");

    int n = 0;
    while (1) {
        int seq = n & 0xffff;
        ++seq;
        n = seq & 0xffff;
    }
    return n;
}
