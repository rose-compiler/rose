void reachable(void) {}
void unreachable(void) {}

int f(int a) {
    reachable();
    return a > 0 ? f(a-1) : 0;
}

// This tests recursion. Reachable should be found exactly 6 times
void test() {
    f(5);
}

int main() {
    return 0;
}
