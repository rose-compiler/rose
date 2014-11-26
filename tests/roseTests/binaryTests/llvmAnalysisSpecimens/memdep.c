/* OPT: -memdep */

void _start() {}

void f1(int a, int b) {
    int c;
    int *cp1 = &c;
    int *cp2 = &c;
    
    a = b;

    c = 0;
    *cp1 += 1;
    *cp2 += 2;
}
