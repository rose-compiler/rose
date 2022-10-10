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


struct C : B
{
  void start() override
  {
    B::start(); // resolves to B::start
  }
};

int main()
{
  C c;
}
