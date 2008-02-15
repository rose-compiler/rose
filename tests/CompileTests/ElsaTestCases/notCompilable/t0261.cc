// t0261.cc
// non-template member class of a template class
// needed for ostream

template <class T>
class A {
  struct B {
    int x;
    char y;
    
    int foo() 
    { return sizeof(T) + x; }
    
    int bar();
  };
};

// instantiate class decl
A<int> *aptr;

// instantiate class defn
A<int> a;                

// eager-instantiate all of A<int>
template class A<int>;


template <class T2>
int A<T2>::B::bar()
{
  return sizeof(T2) + y;
}


void f()
{
  // demand-instantiate all of A<float>
  A<float>::B b;
  b.x + b.y;
  b.foo() + b.bar();
}


// EOF
