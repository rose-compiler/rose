// t0463.cc
// ambiguous function template parameter

// Neither syntax below is valid, because function templates
// are not allowed to have default arguments.

template <int m>
struct D {};

namespace N1 {

  enum { A=2 };

              //       +-----params----+
              //                 +---+   +---ret--+ +-name-+ +--defn---+
  //ERROR(1): template < int n = A < 3 > :: D < n > myfunc() { /*...*/ }
}


namespace N2 {

  template <int n>
  struct A {
    enum { D=5 };
  };

  enum { n=6 };


              //       +-------params-------------+
              //                 +less-than-expr+
              //                 +-member---+
              //                 +-qual+            +-name-+ +--defn---+
  //ERROR(2): template < int n = A < 3 > :: D < n > myfunc() { /*...*/ }
}


// EOF
