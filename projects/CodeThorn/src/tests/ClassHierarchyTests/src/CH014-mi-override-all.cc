struct A
{
  virtual void f() {}
};


struct B
{
  virtual void f() = 0;
};

struct C : A, B
{
  void f() override {}
};
