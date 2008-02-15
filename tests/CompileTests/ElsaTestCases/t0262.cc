// t0262.cc
// template class with a friend

template <class T>
class A {
  friend class B;
};

A<int> x;


// EOF
