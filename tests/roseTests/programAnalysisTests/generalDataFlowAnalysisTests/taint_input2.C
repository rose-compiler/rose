// Binary and Unary operators
extern int TAINTED;
extern int BOTTOM;
int UNTAINTED = 0;

int F1() {
    int a = TAINTED * 2;
    return a;
}

int F2() {
    return -TAINTED;
}

int F3() {
    int a = ++TAINTED;
    return a;
}

int F4() {
    int a = TAINTED++;
    return a;
}

int F5() {
    return UNTAINTED + TAINTED + BOTTOM;
}
