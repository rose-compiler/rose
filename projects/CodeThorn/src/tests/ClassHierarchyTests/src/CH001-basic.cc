
struct A
{
  virtual void test() {}
};

struct B : A
{
  void test() override {}
};
