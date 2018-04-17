// weird function typedefs used in a class

// originally found in package aplus-fsf

// Assertion failed: dt.funcSyntax, file cc_tcheck.cc line 2228

// ERR-MATCH: Assertion failed: dt.funcSyntax

typedef int funcType1(int);
typedef int *funcType2(int);
typedef int (*funcType3)(int);

struct S1 {
    funcType1 f1;
    funcType2 f2;
    funcType3 f3;
};
