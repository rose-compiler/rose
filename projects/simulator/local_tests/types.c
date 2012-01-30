/* Specimen on which we run some type analyses. */
int g1 = 1;
int *g2 = &g1;
int **g3 = &g2;
int g4[5];

extern int e1;
extern int *e2;
extern int **e3;
extern int e4[5];

/******************************************************************************************************************************
 *                                      SIMPLE TESTS
 ******************************************************************************************************************************/

int main() {
    return 0;
}

/* no pointer */
int simple1(int a) {
    return a;
}

/* one pointer argument */
int simple2(int *a) {
    return *a;
}

/* two pointers: the stack argument and the thing that pointer points to */
int simple3(int **a) {
    return **a;
}

/* no pointer since g1 is defined in this object file */
int simple4() {
    return g1;
}

/* one pointer at a known address */
int simple5() {
    return *g2;
}

/* two pointers both at known addresses */
int simple6() {
    return **g3;
}

/* the address is not an L-value, thus not detected as a pointer */
int* simple7() {
    return &g1;
}

/* one pointer with known value */
int* simple8() {
    return *g3;
}

/* two pointers: one with a known address, the other is the local stack variable */
int simple9() {
    int *s1 = g2;
    return *s1;
}

/* no pointer since the compiler optimizes away the pointer by constant folding base+index*size */
int simple10() {
    return g4[0];
}

/* no pointer since the compiler optimizes away the pointer by constant folding base+index*size */
int simple11() {
    return g4[2];
}

/* one pointer. However, since a[i] and i[a] are the same address, we end up reporting "i" as the pointer.  This is because the
 * calculation of the address of g4[i] involves the instructions that load i into a register, and this load is considered to be
 * the pointer address. */
int simple12(int i) {
    return g4[i];
}

/* no pointer */
int simple13() {
    return e1;
}

/* one pointer at a known address. */
int simple14() {
    return *e2;
}

/* two pointers both at known addresses */
int simple15() {
    return **e3;
}

/* no pointer since there is no dereference */
int* simple16() {
    return &e1;
}

/* one pointer at a known address */
int* simple17() {
    return *e3;
}

/* two pointers: one with a known address, the other is the local stack variable */
int simple18() {
    int *s1 = e2;
    return *s1;
}

/* no pointer */
int simple19() {
    return e4[0];
}

/* no pointer */
int simple20() {
    return e4[2];
}

/* one pointer, "i".  See simple12 explanation. */
int simple21(int i) {
    return e4[i];
}

/* one code pointer, "f" */
int simple22(int(*f)()) {
    return f();
}

/* one code pointer, "f".  The address of x in the call to f is an R-value. */
int simple23(int(*f)(int*)) {
    int x = 100;
    return f(&x);
}

/* one code pointer, "f".  The "a" pointer is not detected because it is never dereferenced by this function -- it is
 * impossible to tell, looking only at the code generated for this one function, that "a" is a pointer. */
int simple24(int(*f)(int*), int *a) {
    return f(a);
}

/* detects both pointers: code pointer "f", data pointer "a" */
int simple25(int(*f)(int), int *a) {
    return f(*a);
}


/******************************************************************************************************************************
 *                                      TESTS WITH CONTROL-FLOW STATEMENTS
 ******************************************************************************************************************************/

/* two pointers on the stack */
int flow1(int n, int *a, int *b) {
    return n ? *a : *b;
}

/* two pointers on the stack */
int flow2(int n, int *a, int *b) {
    if (n)
        return *a;
    return *b;
}

int flow3(int n, int *a, int *b) {
    int *x = n ? a : b;                                         // FIXME: x not detected as pointer
    return *x;
}

/* one pointer on the stack */
int flow4(int n, int *a) {
    int i, retval=0;
    for (i=0; i<n; ++i)
        retval += *a;
    return retval;
}

int flow5(int n, int *a, int *b)
{
    int i, retval=0;
    for (i=0; i<n; ++i)
        retval += (i%2) ? *a : *b;                              // FIXME: only detects 
    return retval;
}
