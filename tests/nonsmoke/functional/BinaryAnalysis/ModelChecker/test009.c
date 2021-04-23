void reachable(void) {}
void unreachable(void) {}

int test(int *p) {
    return *p; /* this MAY be a nullptr dereference. */
}

int main() {
    return 0;
}
