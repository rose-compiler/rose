// 12.6.2 para 2, second example

struct A { A(); };
struct B: public virtual A {};
struct C: public A, public B { C(); };
//ERROR1: C::C(): A() {}           // ill-formed: which A?

