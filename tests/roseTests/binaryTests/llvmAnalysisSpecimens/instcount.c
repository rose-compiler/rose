/* OPT: -instcount */

int a, b, c;

int f() {
    int i;

    for (i=0; i<10; ++i)
        a = i*b / 2 + c;

    return a;
}

void _start() {}
