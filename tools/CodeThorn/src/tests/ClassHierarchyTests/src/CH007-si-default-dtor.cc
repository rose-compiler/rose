struct A
{
  virtual ~A() = 0;
};

struct B : A
{
  ~B() override = default;

  void test(B) {}
};


