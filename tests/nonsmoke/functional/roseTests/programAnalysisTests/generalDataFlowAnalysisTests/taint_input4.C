// Integer pointers

extern int TAINTED_int;
extern int* TAINTED_ptr;

int *F1() {
    return TAINTED_ptr;
}

int F2() {
    return *TAINTED_ptr;
}

int F3() {
    return TAINTED_ptr[0];
}

#ifdef INCLUDE_BROKEN_TESTS
// This doesn't work because the initialization on line 3 marks "b" as untainted and the assignment on line 4 (properly) does
// not mark "b" (the pointer) as tainted, but it also doesn't mark the expression "*b" as tainted.  The "*b" in the return
// statement is a unary op applied to an untainted variable, whose result is also (incorrectly) untainted.
int F4() {                      // 1
    int a = 0;                  // 2
    int *b = &a;                // 3
    *b = TAINTED_int;           // 4
    return *b;                  // 5
}
#endif

#ifdef INCLUDE_BROKEN_TESTS
// This doesn't work because the genericDataflow doesn't figure out that "b" and "a" are aliases.
int F5() {
    int a = 0;
    int *b = &a;
    *b = TAINTED_int;
    return a;
}
#endif

// The address of a variable is untainted when the stack pointer is untainted, even if the variable contains a tainted value.
// Since we don't know the taintedness of the stack pointer (i.e., bottom) then the address of a stack variable is unknown
// (bottom).
int *F6() {
    int a = TAINTED_int;
    return &a;
}
