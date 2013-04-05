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
int simple01(int a) {
    return a;
}

/* one pointer argument */
int simple02(int *a) {
    return *a;
}

/* two pointers: the stack argument and the thing that pointer points to */
int simple03(int **a) {
    return **a;
}

/* statically linked:
 *      no pointer since g1 is never dereferenced in this function.
 * dynamically linked:
 *      one pointer associated with finding the address of g1 via call to __i686.get_pc_thunk.cx
 *      (which is not actually analyzed) and then adding some value to ECX and dereferencing it
 *      to get the value in g1.   The expression will be in terms of the initial value of the ECX
 *      register, such as (add orig_ecx 5352) */
int simple04() {
    return g1;
}

/* statically linked:
 *      one pointer at a concrete address.
 * dynamically linked:
 *      Two pointers: one to obtain the value of g2 based on its dynamic address, and the second
 *      to dereference this value and obtain the return value for the function.   The first
 *      of these pointers will be in terms of the original ECX value. */
int simple05() {
    return *g2;
}

/* statically linked:
 *      two pointers, one of which has a known address. The second address is not known because
 *      the analysis starts with an empty memory state (each address contains an unknown value).
 *      Another way of thinking of it is that even though we assigned "g3=&g2" above, this
 *      function cannot assume that g3 still contains that value.
 * dynamically linked:
 *      three pointers: one to obtain the value of g3 based on its dynamica address, and the
 *      other two resulting from the double dereference. */
int simple06() {
    return **g3;
}

/* the address is not an L-value, thus not detected as a pointer */
int* simple07() {
    return &g1;
}

/* statically linked:
 *      one pointer with a concrete value.
 * dynamically linked:
 *      two pointers: one to find the address of g3 so we can get its value, and the
 *      second one when we dereference that value to obtain the function return value. */
int* simple08() {
    return *g3;
}

/* statically linked:
 *      two pointers: one is the address of global "g2" (a concrete value) and
 *      the other is the local variable "s1".  Both are detected by the single dereference
 *      since both are used to compute the address of the dereference.
 * dynamically linked:
 *      three pointers.  Two pointers are like for the static case except now one of them
 *      has a symbolic value.  The third pointer results from the indirect memory read
 *      to obtain the value stored in "g2". */
int simple09() {
    int *s1 = g2;
    return *s1;
}

/* statically linked:
 *      no pointer since the compiler optimizes away the pointer by constant folding of
 *      base+index*size.
 * dynamically linked:
 *      one pointer to obtain the value of g4 by dereferencing an address based on the
 *      value of ECX obtained from __i686.get_pc_thunk.cx. */
int simple10() {
    return g4[0];
}

/* statically linked:
 *      no pointer since the compiler optimizes away the pointer by constant folding of
 *      base+index*size.
 * dynamically linked:
 *      one pointer to obtain the value of g4 by dereferencing an address based on the
 *      value of ECX obtained from __i686.get_pc_thunk.cx. */
int simple11() {
    return g4[2];
}

/* statically linked:
 *      one pointer, the argument "i".  This is considered to be a pointer because it
 *      participates in the calculation of the the address that's eventually dereferenced.
 *      The "g4" address is not detected as a pointer because it's constant.
 * dynamically linked:
 *      two pointers: the argument "i" as in the static case, and also the address of
 *      "g4" as a function of the ECX value obtained from __i686.get_pc_thunk.cx to
 *      resolve the dynamic linkage. */
int simple12(int i) {
    return g4[i];
}

/* statically linked:
 *      no pointer
 * dynamically linked:
 *      one pointer that's a function of the ECX value returned by __i686.get_pc_thunk.cx
 *      used to find the address of "e1", which is then derferenced to obtain its value. */
int simple13() {
    return e1;
}

/* statically linked:
 *      one pointer, the address of global "e2"
 * dynamically linked:
 *      two pointers: one pointer address is a function of the ECX value returned by
 *      __i686.get_pc_thunk.cx and the other from dereferencing e2's value. */
int simple14() {
    return *e2;
}

/* statically linked
 *      two pointers: one is the concrete address of "e3" and the other is the
 *      symbolic address that is e3's value.  The value is symbolic since the
 *      analysis cannot assume that "e3==&e2" (see types_extern.c) still holds.
 * dynamically linked:
 *      three pointers: two as above, except the address of "e3" is symbolic.
 *      An additional pointer is from dereferencing the ECX value returned by
 *      __i686.get_pc_thunk.cx to resolve dynamic linking. */
int simple15() {
    return **e3;
}

/* no pointer since there is no dereference */
int* simple16() {
    return &e1;
}

/* statically linked:
 *      one pointer at a concrete address.
 * dynamically linked:
 *      two pointers: one is the symbolic address of "e3" as a function of the
 *      ECX value returned by __i686.get_pc_thunk.cx to resolve dynamic linking,
 *      and the other is the value of e3 used in the dereference to get the function
 *      return value. */
int* simple17() {
    return *e3;
}

/* statically linked:
 *      two pointers: the concrete address of "e2" and the local variable "s1".
 * dynamically linked:
 *      three pointers: two as above (except the address of "e2" is symbolic) and
 *      the third is the ECX value returned by __i686.get_pc_thunk.cx to resolve
 *      dynamic linking. */
int simple18() {
    int *s1 = e2;
    return *s1;
}

/* statically linked:
 *      no pointer because the address of e4 is a constant.
 * dynamically linked:
 *      one pinter: the ECX value returned by __i686.get_pc_thunk.cx to resolve
 *      dynamic linking. */
int simple19() {
    return e4[0];
}

/* statically linked:
 *      no pointer since the address of e4 is a constant and the compiler did
 *      constant folding.
 * dynamically linked:
 *      one pointer, the address of e4 as a function of ECX returned by
 *      __i686.get_pc_thunk.cx to resolve dynamic linking. */
int simple20() {
    return e4[2];
}

/* statically linked:
 *      one pointer, "i".  As with simple12(), "i" is considered a pointer
 *      because it participates in the calculation of the address that's
 *      eventually dereferenced.
 * dynamically linked:
 *      two pointers: "i" as above, plus a function of the ECX value returned
 *      by __i686.get_pc_thunk.cx to resolve the dynamic address of "e4". */
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
int flow01(int n, int *a, int *b) {
    return n ? *a : *b;
}

/* two pointers on the stack */
int flow02(int n, int *a, int *b) {
    if (n)
        return *a;
    return *b;
}

/* two pointers on the stack; one local variable */
int flow03(int n, int *a, int *b) {
    int *x = n ? a : b;
    return *x;
}

/* one pointer on the stack */
int flow04(int n, int *a) {
    int i, retval=0;
    for (i=0; i<n; ++i)
        retval += *a;
    return retval;
}

/* both arguments are detected as pointers. */
int flow05(int n, int *a, int *b)
{
    int i, retval=0;
    for (i=0; i<n; ++i)
        retval += (i%2) ? *a : *b;
    return retval;
}

/* both arguments are detected as pointers. */
int flow06(int n, int *a, int *b)
{
    int i, retval=0;
    for (i=0; i<n; ++i) {
        if (i % 2) {
            retval += *a;
        } else {
            retval += *b;
        }
    }
    return retval;
}

/* three pointers detected: a, b, ptr. */
int flow07(int n, int *a, int *b)
{
    int *ptr = (int*)0;
    if (0==n) {
        ptr = a;
    } else {
        ptr = b;
    }
    return *ptr;
}

    
    
        
