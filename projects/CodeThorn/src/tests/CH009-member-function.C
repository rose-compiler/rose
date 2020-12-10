struct A
{
  virtual 
  void start() {}
  
  void init();
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

void A::init() 
{
  // candidates are: A::start, B::start, C::start
  start();
  this->start(); 
}

int main()
{
  A a;
}
