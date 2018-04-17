// t0385.cc
// overload resolution of qualified nondependent name

namespace N {
  void foo(int);         // line 5
  void foo(int,int);     // line 6
}

template <class T>
struct C {
  void bar()             // line 11
  {
    N::foo(3,3);         // *second* foo
  }
};

void baz(C<int> &c)      // line 17
{
  c.bar();
}

asm("collectLookupResults foo=6 bar=11 c=17");

// EOF
