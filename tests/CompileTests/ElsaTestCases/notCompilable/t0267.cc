// t0267.cc
// is this a valid template definition?

// g++ does not like it, and I don't want to like it either.  However,
// it is debatable whether this is legal since it seems that a valid
// specialization *can* be generated (14.6 para 7).  14.5.1.1 is not
// very clear on how an implementation is to associate an out-of-line
// definition with the declaration.

template <class T>
struct A {
  int foo(typename T::type1 x);      // use 'type1' in decl
};

template <class T>
int A<T>::foo(typename T::type2 x)   // use 'type2' in defn
{
  return 1;
}

struct B {
  typedef int type1;                 // for *this* template argument, 'type1'
  typedef int type2;                 // and 'type2' are the same!
};

void f()
{
  A<B> a;                            // so this is a valid specialization (?)
  a.foo(3);
}


// EOF
