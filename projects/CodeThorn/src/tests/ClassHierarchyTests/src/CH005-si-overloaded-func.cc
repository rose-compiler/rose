struct A
{
  virtual void f() & = 0;
  virtual void f() const & {}
  virtual void f() && {}
};


struct B : A
{
  void f() & override {}

  void test(B) {}
};

