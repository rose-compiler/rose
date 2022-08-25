struct A
{
  virtual ~A() {}
};

struct B : A
{
  // auto generated virtual dtor
  // void ~B() OVERRIDE {}
};
