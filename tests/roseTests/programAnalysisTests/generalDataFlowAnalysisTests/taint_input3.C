// Arrays

extern int TAINTED;

int F1() {
    int a[3] = {0, TAINTED, 2};
    return a[1];
}

#ifdef INCLUDE_BROKEN_TESTS
// The "a" variable declaration marks "a" as bottom. Line 3 does not change the taintedness of "a" because "a" does not appear
// by itself on the left of the equal sign (see F3 test below). The "a[0]" expression on line 4 is a binary operation
// (SgPntrArrRefExp) whose operands are "bottom" and "untainted", the meet of which is "untainted".  Thus F2 returns an
// untainted value.
int F2() {              // 1
    int a[3];           // 2
    a[0] = TAINTED;     // 3
    return a[0];        // 4
}                       // 5
#endif

#ifdef INCLUDE_BROKEN_TESTS
// The genericDataflow framework knows that the rhs of the assignment is tainted and that the assignment expression as a whole
// is tainted, but does not figure out that 'a' becomes tainted as a side effect.  The side effect is supposed to be handled in
// the software layer above genericDataflow, but I'm not sure how to generically handle all the possible side effects.
// [RPM 2012-09-11].
int F3() {
    int a[3];
    *a = TAINTED;
    return *a;
}
#endif

#ifdef INCLUDE_BROKEN_TESTS
// The genericDataflow framework knows that the rhs of the assignment is tainted and that the assignment expression as a whole
// is tainted, but does not figure out that 'a' becomes tainted as a side effect.  The side effect is supposed to be handled in
// the software layer above genericDataflow, but I'm not sure how to generically handle all the possible side effects.
// [RPM 2012-09-11].
int F4() {
    int a[3];
    a[0] = TAINTED;
    return *a;
}
#endif

int F5() {
    int a[1] = {0};
    return a[TAINTED];
}


