// t0470.cc
// some experiments with friends of class templates

template <class T>
struct A {
  friend int foo(T *t);
  
  // friend that does not use any of the template params
  friend int zoo(int *p);
  
  void member()
  {
    int *q = 0;
    
    // I cannot tell whether these nondependent lookups should
    // find the friends; icc accepts while gcc rejects
    //foo(q);
    //zoo(q);
  }
};

struct B {
  friend int bar(int *p);
};

void g()
{
  // is 'foo' now accessible here?  according to gcc and icc, no
  int *p = 0;
  //ERROR(1): foo(p);

  // but 'bar' is, right?  yes
  bar(p);

  // what about 'zoo'?  gcc and icc again say no
  //ERROR(2): zoo(p);
}
