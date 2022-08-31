
struct A
{
  virtual void f() {}

  void test(A) {}
};

struct B : A
{
  void f() override {}

  void test(B) {}
};
