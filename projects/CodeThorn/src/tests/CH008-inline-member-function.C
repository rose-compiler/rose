struct A
{
  virtual 
  void start() {}
  
  void init() 
  {
    // candidates are: A::start, B::start, C::start
    start();
    this->start(); 
  }
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
}
