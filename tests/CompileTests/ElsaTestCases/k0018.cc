// member function taking an array argument with default value __null

// Assertion failed: isArrayType(), file cc_type.cc line 870

// originally found in package anjuta

// ERR-MATCH: Assertion failed: isArrayType

struct S {
    // sm: __null is a GNU thing, and the test was failing due to the
    // presence of an initializer (rather than anything having to do
    // with the specific value of that initializer), so I changed it
    // to "0"
    void f(int a[] = 0 /*__null*/);
};
