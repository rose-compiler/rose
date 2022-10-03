struct A
{
  virtual ~A() = 0;
};

struct B : A
{
  // auto generated virtual dtor
  // void ~B() OVERRIDE {}
  
  void test(B) {}
};
