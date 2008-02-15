// t0224.cc
// template with conversion operator yielding dependent type

template <class T>
struct A {};

template <class T>
struct B {
  operator A<T>* ()
    { return 0; }
    
  // and a variable with dependent type ..
  A<T> &foo;
};

void f()
{
  B<int> b;
  b.operator A<int>* ();
}
