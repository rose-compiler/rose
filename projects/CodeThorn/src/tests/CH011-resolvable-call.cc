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
  A a;
  
  a.start();
}
