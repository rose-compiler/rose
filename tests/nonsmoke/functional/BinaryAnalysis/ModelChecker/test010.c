void reachable(void) {}
void unreachable(void) {}

void test(int a) {
    int *p = 0;
    *p = a; /* this MUST be a nullptr dereference. */
}

int main() {
    return 0;
}
