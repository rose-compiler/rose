void reachable(void) {}
void unreachable(void) {}

int test() {
    int *p = 0;
    return *p; /* this MUST be a nullptr dereference. */
}

int main() {
    return 0;
}
