// 13.3.3.1.4.cc

struct A {};
struct B : public A {} b;
int f(A&);                         // line 5
int f(B&);                         // line 6

int i = __testOverload(f(b), 6);   // Calls f(B&), an exact match, rather than
                                   // f(A&), a conversion
