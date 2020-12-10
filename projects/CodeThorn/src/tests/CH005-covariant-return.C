
struct A 
{
  virtual
  A* create()
  {
    return new A;
  }
};

struct B : A
{
  B* create() override
  {
    return new B;
  }
};

struct C : A
{
  C* create() override
  {
    return new C;
  }
};

int main()
{
  A* a = new A;

  // Class Hierarchy analysis: candidates are A::start, B::start, or C::start
  // Precise analysis: A::start
  A* z = a->create();
}
