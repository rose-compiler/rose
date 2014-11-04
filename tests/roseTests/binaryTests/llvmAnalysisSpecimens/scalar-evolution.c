/* OPT: -scalar-evolution */

void _start() {
    int i, a, b;

    for (i=0; i<10; ++i)
        a = i*2 + b;
}
