// t0428.cc
// dependent qualified self-name name in an enumerator initializer
// in a template partial spec

template <typename T, typename U>
struct B;

template <typename W, typename V>
struct A;

template <typename V>
struct A<int, V> {
  enum { z = 0 };
};

template <typename T, typename U, typename V>
struct A<B<T, U>, V> {
  enum { x = A<U, V>::z };
  enum { z = 1 };
};

A<B<int, int>, int> a;
