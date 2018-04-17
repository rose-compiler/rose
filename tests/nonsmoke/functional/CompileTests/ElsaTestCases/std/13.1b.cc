// 13.1b.cc

typedef int Int;

void f(int i);
void f(Int i);                    // OK: declaration of f(int)
void f(int i) { /* ... */ }       
//ERROR(1): void f(Int i) { /* ... */ }    // error: redefinition of f(int)

