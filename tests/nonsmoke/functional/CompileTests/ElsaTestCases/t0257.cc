// t0257.cc
// explicit instantiation request of a class that has
// a templatized constructor

template <class S>
struct A {
  template <class T>
  A(T t) { }
};

template struct A<char>;
