void reachable(void) {}
void unreachable(void) {}

void test() {
    while (1) /*void*/;
    unreachable();
}

int
main() {
    return 0;
}
