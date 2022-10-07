struct A
{
  virtual void f() = 0;
};

struct B : A
{
  void f() override {}

  void test(B) {}
};

struct C : A
{
  void f() override = 0;
};


struct D : B,C
{  
};

struct E : D
{
  void f() override {}

  void test(E) {}
};

