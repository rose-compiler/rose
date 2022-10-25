struct A
{
  virtual 
  void start() {}
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
  A* a = new B;

  // resolves to A::start
  a->A::start();
}
