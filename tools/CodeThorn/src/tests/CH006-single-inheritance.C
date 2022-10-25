struct A 
{
  virtual void start()
  {}
};

struct B : A
{
  void start() override
  {}
};

struct C : A
{
  void start() override
  {}
};

int main()
{
  A* a = new A;

  // Class Hierarchy analysis: candidates are A::start, B::start, or C::start
  // Precise analysis: A::start
  a->start();
}
