// t0161.cc
// multiple introduction of alias base class members

struct B {
  int f();
};    

struct D : B {
  using B::f;
  //ERROR(1): using B::f;
};
