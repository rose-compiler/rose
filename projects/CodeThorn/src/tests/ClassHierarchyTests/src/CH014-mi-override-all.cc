struct A
{
  virtual void f() {}

  void test(A) {}
};


struct B
{
  virtual void f() = 0;
};

struct C : A, B
{
  void f() override {}

  void test(C) {}
};
