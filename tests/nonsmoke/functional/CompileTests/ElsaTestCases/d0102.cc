// error: there is no type called `Iter'

template<class C> struct A {
  typedef A<C> Iter;
};

template<class C> struct B : A<C> {
  typename A<C>::Iter &operator = (const typename A<C>::Iter &orig) {}
};
