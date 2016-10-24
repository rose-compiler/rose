// t0175.cc
// test from jrvb: definition of a static data member of a template
// outside the body of the template-class declaration

template <class T>
class C {
  static int x;
};

template <class T>
int C<T>::x = 0;
