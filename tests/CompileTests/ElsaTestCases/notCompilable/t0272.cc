// t0272.cc
// counterpart to t0271.cc: *non*-dependent base class names hide others

// base class; will be *non*-dependent
struct Base {
  int S;
  int I;

  // I made this overloaded to force Elsa to compare the
  // arguments to the parameters
  int foo(int);
  int foo(float);
};

// this typedef is what is seen when 'I' is used, below
typedef int I;

// and one more for a function call
int foo(int, int);
int foo(float, float);

// derived struct
template <class S>
struct Derived : Base {
  // Since we inherit from Base, and Base has a member called
  // 'S', it *does* hide the template parameter 'S'.

  //ERROR(1): S x;     // only works if 'S' is a type

  //ERROR(2): I y;     // same deal

  void bar() {
    foo(I);            // non-dependent; uses Base::I and Base::foo(int)
    foo(S);            // non-dependent; uses Base::S and Base::foo(int)
  }
};

// instantiate it
template class Derived<float>;


// EOF




