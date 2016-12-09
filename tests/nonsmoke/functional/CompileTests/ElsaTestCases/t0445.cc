// t0445.cc
// DQTs in non-function declarators

template <class T>
struct A {
  struct B {};
  
  static B *array[2];
};

template <class T2>
typename A<T2>::B *A<T2>::array[2] = { 0,0 };

void foo()
{
  A<int>::array[0] = A<int>::array[1];
}
