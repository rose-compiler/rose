void reachable(void) {}
void unreachable(void) {}

void test() {
    int a = 1;
    if (a) {
        reachable();
    } else {
        unreachable();
    }
}

int main() {
    return 0;
}
