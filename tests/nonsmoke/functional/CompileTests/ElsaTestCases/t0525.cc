// t0525.cc
// use of a dependent expression in a default argument

template <class T>
struct A {
  enum { foo=3 };
};

template <class T, int n = A<T>::foo>
struct B {
  // no 'x' here
};

template <class T>
struct B<T,3> {
  B();        // silence icc warning ..
  int x;      // specialization has it
};

int foo()
{
  B<int> b;

  // have to have evaluated the default arg correctly, despite the
  // fact that it was specified using a dependent expression, to get
  // the specialization that has 'x'
  return b.x;
}



