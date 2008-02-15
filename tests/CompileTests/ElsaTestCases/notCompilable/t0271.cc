// t0271.cc
// template parameter hides *dependent* base class member
// this is what is going wrong in t0270.cc

// base class; will be *dependent*
template <class T>
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
struct Derived : Base<S> {
  // Since we inherit from Base<S>, and Base<S> has a member called
  // 'S', it might naively hide the template parameter 'S'.  In fact,
  // if the base class were not dependent on the template parameter,
  // that would be the correct semantics (14.6.1 para 7).  However,
  // since it *is* dependent, the template parameter name takes
  // precedence (14.6.2 paras 3, 4).

  S x;     // only works if 'S' is a type, namely the template parameter

  I y;     // same deal

  void bar() {
    foo(2,3);     // non-dependent; uses ::foo(int, int)
    
    S s;
    foo(s);       // dependent; uses Base<S>::foo
  }
};

// instantiate it
template class Derived<float>;


// EOF
