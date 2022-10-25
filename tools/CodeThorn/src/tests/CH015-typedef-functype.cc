struct A 
{
  typedef int I;
  typedef I (*X) (I);

  virtual X start();
};

struct B : A
{
  typedef int J;
  typedef J (*Z) (J);

  Z start() override;
};

struct C : A
{
  X start() override;
};

int foo(A& a)
{
  a.start();
}
