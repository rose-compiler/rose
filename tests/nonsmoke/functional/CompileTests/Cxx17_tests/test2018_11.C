// Allow constant evaluation for all non-type template arguments

template<int* p> class X { };

int a[10];
struct S { int m; static int s; } s;

// X<&a[2]> x3; // error: address of array element
// X<&s.m> x4;  // error: address of non-static member
// X<&s.s> x5;  // error: &S::s must be used OK: address of static member
X<&S::s> x6; // OK: address of static member
