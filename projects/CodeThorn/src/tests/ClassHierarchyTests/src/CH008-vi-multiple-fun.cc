struct A
{
  virtual void f() = 0;
};

struct B : virtual A
{
  void f() override {}
};

struct C : virtual A
{
  void f() override = 0;
};


struct D : B,C
{  
  void f() override = 0;
};

struct E : D
{
  void f() override {}
};


int main()
{
  E d;
}
