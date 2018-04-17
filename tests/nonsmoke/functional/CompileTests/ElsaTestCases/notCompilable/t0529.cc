// t0529.cc
// constness of member function affects data elements

struct A {
  void g(int);           // line 5
  void g() const;        // line 6      right one
  void g();              // line 7
};


struct B {
  A a;
  mutable A a2;

  void f() const
  {
    __testOverload(a.g(), 6);
  }

  void h()
  {
    A const b;
    __testOverload(b.g(), 6);
  }
  
  // using the mutable member, it's not const
  void k() const
  {
    a2.g(3);
  }
};
