struct A
{
  virtual int f(int**) = 0;
};

struct B : virtual A
{
  int f(int* x[]) override { return **x;}

  void test(B) {}
};

struct C : virtual B
{
  int f(int** x) override { return **++x;}

  void test(C) {}
};

