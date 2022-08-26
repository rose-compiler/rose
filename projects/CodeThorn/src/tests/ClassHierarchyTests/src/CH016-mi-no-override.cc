struct A
{
  virtual void f() {}
};


struct B
{
  virtual void f() {}
};

struct C : A, B
{
};
