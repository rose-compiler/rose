// t0246.cc
// invoke an inline-defined static method in a specialization
// from nsAtomTable.i, copy_string


template <class T>
struct A {
};

template <class S>
struct A<S*>
{
  static void method()
  { }
};

void foo2()
{
  A<int*> a;
  a.method();
}
