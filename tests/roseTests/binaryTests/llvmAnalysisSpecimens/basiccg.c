/* OPT: -basiccg */
void _start() {}

void f1() {}

void f2() {
    f1();
}

void f3() {
    f2();
    f1();
}

void f4() {
    f3();
}

void f5() {
    f3();
    f4();
}
