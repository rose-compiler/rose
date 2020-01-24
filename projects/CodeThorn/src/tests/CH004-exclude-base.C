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

struct D : A, B, C
{ 
  void start() override
  {}
};


int main()
{
  B* b = new B;

  // Class Hierarchy analysis: candidates are B::start, C::start, or D::start
  // Precise analysis: B::start
  b->start();
}
