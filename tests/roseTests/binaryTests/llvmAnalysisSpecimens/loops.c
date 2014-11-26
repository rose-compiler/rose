/* OPT: -loops */

int random() {
    return 1;
}

void _start() {
    int i, a, b;

    for (i=0; i<10; ++i) {
        a += i * b;
    }

    while (i<20) {
        a += ++i * b;
    }

    do {
        a += ++i * b;
    } while (i<30);

    while ((i < 40 && random()) || (i < 41 && random()) || random()) {
        a += ++i;
    }
}
