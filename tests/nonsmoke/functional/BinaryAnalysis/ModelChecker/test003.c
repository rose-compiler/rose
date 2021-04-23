void reachable(void) {}
void unreachable(void) {}

/* Without a K bound this will run forever */
void test(int a) {
    for (int i = 0; i < 1000000000; ++i)
        ++a;                                            /* body state changes each iteration */
    unreachable();                                      /* K limit will have prevented us from reaching this */
}

int main() {
    return 0;
}
