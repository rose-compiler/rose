/* compile as:
 *   gcc -static -m32 -nostdlib -nostartfiles -O0 -g -Wall -o concolic-specimen-01 -Wl,-e_start concolic-specimen-01-source.c
 */
void halt() {
    asm("hlt");
}

void exit(int n) {
    asm("int $0x80"
        : /* no output */
        : "D"(n), "a"(252)
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
