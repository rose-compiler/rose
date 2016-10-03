// cc.in25
// inner classes

class A {
public:
  class B {
  public:
    static int x;
    int y;
    int f();
    double g();
  };

  int func(B &b);
};

// verify that declarators can see the class scope
int A::func(B &b)
{
  return 6;
}


int main()
{
  int d;
  d = A::B::x;

  A::B b;
  d = b.y;
}

int A::B::f()
{
  return 6;
}

//ERROR(1): int A::B::h() {   return 66; }   // undeclared

double x;

double A::B::g()
{
  return ::x;
}

