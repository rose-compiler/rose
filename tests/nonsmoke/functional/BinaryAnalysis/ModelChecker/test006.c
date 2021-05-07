void reachable(void) {}
void unreachable(void) {}

/* Should be 16 calls to reachable. */
void recursive(int a) {
    if (a > 0) {
        recursive(a - 1);
        recursive(a - 1);
    } else {
        reachable();
    }
}

void test() {
    recursive(4);
}

int main() {
    return 0;
}
