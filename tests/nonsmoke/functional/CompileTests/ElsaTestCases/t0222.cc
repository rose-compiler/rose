// t0222.cc
// scope introduction from qualified-id, with template classes

// primary
template <class S>
struct A {
  enum E { value };
  int func(E e);
};

// partial specialization
template <class T>
struct A<T*> {
  enum F { value2 };
  int func(F f);
};


// definition of primary func
template <class S>
int A<S>::func(E e)    // note that 'E' refers to 'A<S>::E'
{
  return 3;
}

// definition of specialization func
template <class T>
int A<T*>::func(F f)   // note that 'F' refers to 'A<T*>::F'
{
  return 4;
}


// instantiate them
void foo()
{
  A<int> a1;
  a1.func(A<int>::value);
  
  A<int*> a2;
  a2.func(A<int*>::value2);
}


// EOF
