// t0474.cc
// instantiating a pre-declared friend template

// pre-declare A
template <class T>
struct A;

// now I can pre-declare 'foo'
template <class T>
int foo(T t);

template <class T>
struct A {
  // this says that there is a template function called 'foo'
  // (Q: what determines how it is parameterized?) that is
  // a friend of this class
  friend int foo<> (T t);
};

// here I define that template
template <class T>
int foo(T t)
{
  return sizeof(T);
}

// and now I want to instantiate it
template int foo(float t);


// ---------------------
// now repeat the above, except for an operator function
template <class T>
struct B;

template <class T>
int operator+ (int x, B<T> t);

template <class T>
struct B {
  friend int operator+ <> (int, B<T> t);
};

template <class T>
int operator+ (int x, B<T> t)
{
  return x + sizeof(B<T>);
}

template int operator+ (int x, B<float> t);


// EOF
