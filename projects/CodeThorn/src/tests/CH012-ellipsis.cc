struct A 
{
  virtual void start(...);
};

struct B : A
{
  void start(...) override;
};

struct C : A
{
  void start(); // does not override
};

int foo(A& a)
{
  a.start();
}
