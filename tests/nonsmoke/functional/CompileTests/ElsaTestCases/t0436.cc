// t0436.cc
// scope in which the class of a ptr-to-member is looked up

struct B {
  int member;
  int *p;
  int f();
};

int func();

struct A {        
  // interloper!  If I do the lookups of the names preceding
  // "::*" in the scope of A, I will find this one, instead of
  // ::B, which is the right one
  struct B {
    int memb;
  };

  int ::B::* f1();
  int ::B::* f2();

  int* ::B::* f3();

  int (*f4())();;

  int (::B::* f5())();

  int B::* g1();
  int B::* g2();
};

int ::B::* A::f1()
{
  return &::B::member;
}

int B::* A::f2()
{
  return &::B::member;
}

int* B::* A::f3()
{
  return &::B::p;
}

int (*A::f4())()
{
  return &func;
}

int (B::* A::f5())()
{
  return &::B::f;
}

int A::B::* A::g1()
{
  return &B::memb;
}

// should *not* find A::B without qualifier
//ERROR(1): int B::* A::g2()
//ERROR(1): {
//ERROR(1):   return &B::memb;
//ERROR(1): }


