// t0294.cc
// overloaded operator -> yields a type that is then involved
// in overload resolution

// needed for Mozilla xpcwrappednative.i


struct A {
  int x;
};


struct Ptr {
  A* operator -> ();
};


void func(int);
void func(float);


void f()
{
  Ptr p;
  func(p->x);
}
