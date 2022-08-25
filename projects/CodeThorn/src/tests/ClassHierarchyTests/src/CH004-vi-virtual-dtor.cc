struct A
{
  virtual ~A() = 0;
};

struct B : virtual A
{
  ~B() override {}
};

struct C : virtual A
{
  ~C() override {}
};


struct D : B,C
{
};


int main()
{
  D d;
}
