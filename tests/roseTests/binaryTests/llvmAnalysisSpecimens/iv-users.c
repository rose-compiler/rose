/* OPT: -iv-users */

int f1(int b) {
    int i, a=0;
    for (i=0; i<10; ++i)
        a += i * b + 1;
    return a;
}

void _start() {}
