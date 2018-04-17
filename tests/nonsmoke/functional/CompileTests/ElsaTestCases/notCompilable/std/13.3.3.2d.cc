// 13.3.3.2d.cc

struct A {};
struct B : public A {};
struct C : public B {};
C *pc;
int f(A *);                         // line 7
int f(B *);                         // line 8
int i = __testOverload(f(pc), 8);   // Calls f(B*)
