// t0426.cc
// more complicated version of t0425.cc:                 

// use of self-name with different arguments as qualifier, and
// the qualified name then becomes a typedef


template <class T, class U>
struct B {
};

template <class T, class U = int>
struct A {
  typedef typename A<U>::W V;
  typedef B<T, V> W;
};

template <>
struct A<int, int> {
  typedef int W;
};

A<char, char> a;
