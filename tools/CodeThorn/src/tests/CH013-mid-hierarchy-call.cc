struct A 
{
  virtual void start()
  {}
};

struct B : A
{
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
  
  b->start();
}
