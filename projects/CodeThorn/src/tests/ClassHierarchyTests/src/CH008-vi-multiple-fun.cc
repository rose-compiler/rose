struct A
{
  virtual void f() = 0;
};

struct B : virtual A
{
  void f() override {}

  void test(B) {}
};

struct C : virtual A
{
  void f() override = 0;
};


struct D : B,C
{  
  void f() override = 0;
};

struct E : D
{
  void f() override {}

  void test(E) {}
};

