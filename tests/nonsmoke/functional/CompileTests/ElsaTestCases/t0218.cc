// t0218.cc
// template class with template constructor
// needed for iostream

template <class S>
struct A {
  // inline templatized constructor
  template <class T>
  A(A<T> const &obj)
  {}
};

//  void foo()
//  {
//    A<int> a;        // instantiate A<int>
//    A<float> b(a);   // instantiate A<float> and A<float>::A<int>()
//  }


// EOF
