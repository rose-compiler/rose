// t0475.cc
// explicit specialization of a ctor, then use it

template <class T>
struct A {
  A(int);
  A(int, T);
  A(int*);

  int foo(int);
  int foo(int, T);
  int foo(int*);
};

// definition in general of ctor
template <class T>
A<T>::A(int, T)
{ A<T*>::glarg(); }           // never instantiated

// explicit specialization of ctor; overrides the general
// definition at <float>
template <>
A<float>::A(int, float) {}

// similar for an ordinary function
template <class T>
int A<T>::foo(int, T)
{ return A<T*>::glarg(); }    // never instantiated

template <>
int A<float>::foo(int, float)
{ return 2; }

// use it
void test_a()
{
  A<float> a(1, 2.3);
  a.foo(1, 2.3);
}


// ------------------
// same thing, but explicit spec comes first
template <class T>
struct B {
  B(int);
  B(int, T);
  B(int*);

  int foo(int);
  int foo(int, T);
  int foo(int*);
};

// explicit specialization of ctor
template <>
B<float>::B(int, float) {}

// definition in general of ctor
template <class T>
B<T>::B(int, T)
{ B<T*>::glarg(); }           // never instantiated

template <>
int B<float>::foo(int, float)
{ return 2; }

template <class T>
int B<T>::foo(int, T)
{ return B<T*>::glarg(); }    // never instantiated

// use it
void test_b()
{
  B<float> b(1, 2.3);
  b.foo(1, 2.3);
}

