// t0429.cc
// more with qualified destructor names
// (variations on 3.4.3p5.cc)

struct A {
  ~A();
};
typedef A AB;
void foo()
{
  AB *p = 0;
  p->AB::~AB();
  p->::AB::~AB();
}

namespace N {
  struct B {};
  typedef B BB;
}
void bar()
{
  N::B *b = 0;
  N::BB *bb = 0;
  b->N::~B();          // both icc and gcc reject this
  b->N::~BB();         // and this
  b->N::B::~B();
  b->N::BB::~B();
  b->N::B::~BB();
  b->N::BB::~BB();
}


// EOF
