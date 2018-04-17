// t0233.cc
// demonstrate flaw in interaction between overload resolution and
// instantiation

template <class T>
struct A {
  int foo(T *t, T *u)      // overloaded declaration
  {
    // if instantiated with T=int, this statement causes an error
    return T::doesNotExist;
  }

  int foo(T *t)
  { return 1; }
};

void f()
{
  A<int> a;
  int *x;

  // this should only instantiate the *second* foo, but our
  // implementation currently instantiates both, which is wrong
  //
  // actually, right now, it always just instantiates the first,
  // regardless of args, so that's two flaws this demonstrates 
  a.foo(x);
}

// EOF
