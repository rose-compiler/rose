struct X {};
struct Y : X {};


struct A
{
  virtual const X& gen() = 0;
};


struct B : A
{
  const Y& gen() override { return Y{}; };

  void test(B) {}
};

