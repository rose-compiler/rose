// Assignment initializers and assignment

extern int TAINTED;

int F1() {
    int a = TAINTED;
    return a;
}

int F2() {
    int a;
    a = TAINTED;
    return a;
}

int F3() {
    int a = TAINTED;
    int b = a;
    return b;
}

int F4() {
    int a, b;
    a = TAINTED;
    b = a;
    return b;
}

int F5() {
    int a;
    a = TAINTED;
    int b = a;
    return b;
}
