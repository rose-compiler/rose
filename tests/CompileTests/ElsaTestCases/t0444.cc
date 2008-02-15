// t0444.cc
// DQTs with non-type args

template <int N>
struct A {
  struct B {};
  
  B *f();
};

template <int M>
typename A<M>::B *A<M>::f()
{
  return new B;
}

void foo()
{
  A<3> a3;
  a3.f();

  A<4> a4;
  a4.f();
}



// EOF
