// error: there is no type called `A'

template<class V> struct C {
  typedef struct A0 A;
};

template<class V> 
struct D : C<V> {
  typename C<V>::A x;
};
