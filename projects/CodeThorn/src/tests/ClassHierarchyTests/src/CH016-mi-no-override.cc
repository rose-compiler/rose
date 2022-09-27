struct A
{
  virtual void f() {}

  void test(A) {}
};


struct B
{
  virtual void f() {}

  void test(B) {}
};

struct C : A, B
{ 
  void test(C) {}
};
