// 13.1e.cc

void h(int());
void h(int (*)());           // redeclaration of h(int())
void h(int x()) { }          // definition of h(int())
//ERROR(1): void h(int (*x)()) { }       // ill-formed: redefinition of h(int())
