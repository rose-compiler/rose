struct A 
{
  typedef int X;

  virtual void start(X);
};

struct B : A
{
  typedef int Y;

  void start(Y) override;
};

struct C : A
{
  void start(int) override;
};

int foo(A& a)
{
  a.start(0);
}
