void reachable(void) {}
void unreachable(void) {}

/* reachable is called twice */
void test(int a) {
    int sum = 0;

    /* Feasible paths will only reach exactly one ++sum */
    if (a) {
        ++sum;
        if (!a)
            ++sum;
    } else {
        ++sum;
        if (a)
            ++sum;
    }

    if (1 == sum) {
        reachable();
    } else {
        unreachable();
    }
}

int main() {
    return 0;
}
