// t0273.cc
// explicitly instantiate a template class that inherits a non-template

struct Base {
};

template <class T>
struct Derived : Base {
  // threw this in too.. though it didn't cause any new problems
  struct Inner {
  };
};

template class Derived<float>;


// EOF
