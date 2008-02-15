// 13.1f.cc

typedef const int cInt;

int f (int);
int f (const int);             // redeclaration of f(int)
int f (int) { /*...*/ }        // definition of f(int)
//ERROR(1): int f (cInt) { /*...*/ }       // error: redefinition of f(int)
