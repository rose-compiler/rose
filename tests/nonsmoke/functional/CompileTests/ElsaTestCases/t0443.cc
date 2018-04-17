// t0443.cc

// error: typedef'd name `U' doesn't refer to a class, so it can't be
// used as a scope qualifier

template <class T>
struct A {
  struct T1 {
    typedef int T2;
  };
};

template <class T>
struct C {};

template <class T>
struct List {};

template <class T>
struct Vector {};

template <class T>
struct B {
  typedef typename A<T>::T1 U;
  typedef typename U::T2 V;
  typedef Vector<List<C<V> > > W;
};
