void reachable(void) {}
void unreachable(void) {}

void test(int *p, int a) {
    *p = a; /* this MAY be a nullptr dereference. */
}

int main() {
    return 0;
}
