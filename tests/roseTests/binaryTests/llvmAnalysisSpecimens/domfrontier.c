/* OPT: -domfrontier */

int rand() {
    return 1;
}

void _start() {
    int a, b, c, d, e, f;
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
}
