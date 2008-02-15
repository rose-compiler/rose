// t0491.cc
// explicit instantiation request with explicit specialization

template <class T, class U>
void foo(T t, U u) {}

template <class T>
void foo(T t, int u) {
  // this would be an error because it is *this* template that
  // the instantiation selects, but T is not a class in that case
  //ERROR(1): return T::foo;
}

template void foo(int,int);

