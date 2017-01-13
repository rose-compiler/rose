// Function pointers

extern int TAINTED_int;
int TAINTED_func();
extern int(*TAINTED_fp)();

int F1() {
    return TAINTED_int;
}

int(*F2())() {
    return TAINTED_fp;
}

#ifdef INCLUDE_BROKEN_TESTS
// Fails an assertion in the genericDataflow callgraph traversal because the code assumes that all function calls are via
// functions rather than pointers to functions.
int F3() {
    return TAINTED_fp(); // calling a tainted function pointer should produce a tainted result
}
#endif

#ifdef INCLUDE_BROKEN_TESTS
// Fails an assertion for the same reason as F3.
int F4() {
    int(*fp)() = F1;    // fp is not itself tainted since the address of F1 is not tainted,
    int a = fp();       // but calling fp should produce a tainted result.
    return a;
}
#endif
