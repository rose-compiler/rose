/* compile as:
 *   gcc -nostdlib -o concolic-specimen-01 concolic-specimen-01.c
 */
void halt() {
    asm("hlt");
}

void exit(int n) {
    asm("int $0x80"
        : /* no output */
        : "D"(n), "a"(231)
        :
        );
}

void f(int x, int y) {
    int z = 2 * y;
    if (1000000 == x) {
        if (x < z) {
            halt();
        }
    }
}

void _start() {
    int x, y;                                           // not initialized
    f(x, y);
    exit(0);
}
