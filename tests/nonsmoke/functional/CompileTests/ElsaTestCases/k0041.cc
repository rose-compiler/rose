// inner struct defined inline in an inner struct not defined inline

// originally found in package kdelibs_4:3.3.1-4

// Assertion failed: (unsigned)i < (unsigned)sz, file ../smbase/array.h line 66

// ERR-MATCH: \QAssertion failed: (unsigned)i < (unsigned)sz\E

struct S1 {
    struct S2;
};

struct S1::S2 {
    struct S3 {
    };
};

S1 s1;
