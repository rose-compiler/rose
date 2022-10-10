struct A 
{
  virtual void start() &;
};

struct B : A
{
  // no overrider
  void start() const &;

  void start() &&;

  void start(int) &;

  void start(...) &;
};

struct C : A
{
  void start() & override;
};

void test(A& a)
{
  a.start();
}
