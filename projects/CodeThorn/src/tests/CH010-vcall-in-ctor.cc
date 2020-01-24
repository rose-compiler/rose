struct A
{
  virtual 
  void start() {}
  
  A() 
  {
    // resolve to A::start
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
