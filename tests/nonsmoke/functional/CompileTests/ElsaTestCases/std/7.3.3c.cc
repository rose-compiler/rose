// 7.3.3c.cc

// this one does not work since member templates are not
// implemented at all--they don't even parse!

//  class A {
//  public:
//    template <class T> void f(T);
//    template <class T> struct X { };
//  };

//  class B : public A {
//  public:
//    using A::f<double>;     // ill-formed
//    using A::X<int>;        // ill-formed
//  };
