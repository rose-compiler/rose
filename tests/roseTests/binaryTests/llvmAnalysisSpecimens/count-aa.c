/* OPT: -count-aa -print-alias-sets */

struct S1 {
    int m1;
};

/* s1.m1 and s2.m1 cannot be aliases */
int f1(void) {
    struct S1 s1, s2;
    s1.m1 = 1;
    s2.m1 = 2;
    return s1.m1 + s2.m1;
}

/* s1->m1 and s2->m1 could be aliases */
int f2(struct S1 *s1, struct S1 *s2) {
    s1->m1 = 1;
    s2->m1 = 2;
    return s1->m1 + s2->m1;
}

/* *m1p and *m2p are aliases */
int f3(void) {
    struct S1 s1;
    int *m1p = &s1.m1;
    int *m2p = &s1.m1;

    *m1p = 1;
    *m2p = 2;
    return *m1p + *m2p;
}

void _start() {}
