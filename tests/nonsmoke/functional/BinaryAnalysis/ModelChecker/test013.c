void reachable(void) {}
void unreachable(void) {}

void test(int a) {
    if (a > 0) {
        test(a-1);
    } else {
        test(a+1);
    }
}

int main() {
    return 0;
}
