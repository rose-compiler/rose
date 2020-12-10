
struct A 
{
  virtual void start()
  {}
};

struct B : virtual A
{
  void start() override
  {}
};

struct C : virtual A
{
  void start() override
  {}
};

struct D : B, C
{ 
  void start() override
  {}
};

int main()
{
  A* a = new A;
 
 // Class Hierarchy analysis: candidates are A::start, B::start, C::start, or D::start
 // Precise analysis: A::start
  a->start ();
}
