struct A {
  struct B {};
  // member function declaration parameters should NOT get cdtors
  void f(B a);
};
