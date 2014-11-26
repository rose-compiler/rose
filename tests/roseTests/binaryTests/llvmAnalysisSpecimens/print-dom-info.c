/* OPT: -print-dom-info */

int rand() {
    return 1;
}

void _start() {
    int a, b, c, d, e, f, i;
    a = 1;
    if (rand()) {
        b = 2;
        while (rand()) {
            c = 3;
        }
        d = 4;
    } else {
        e = 5;
    }
    f = 6;

    for (i=0; i<10; ++i) {
        a += i * b;
    }

    while (i<20) {
        a += ++i * b;
    }

    do {
        a += ++i * b;
    } while (i<30);

    while ((i < 40 && rand()) || (i < 41 && rand()) || rand()) {
        a += ++i;
    }

    i = a;
}
