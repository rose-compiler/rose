void foo1();

class A
{
public:
  virtual bool (A::*f1(char c))(int) = 0;
  virtual bool f2(int) = 0;
  virtual bool f3(int) {}
};

class B : public A
{
public:
  virtual bool (A::*f1(char c))(int)
  {
    return &A::f2;
  }

  virtual bool f2 (int i);
  virtual bool f3 (int);
  virtual bool f4 (bool) {}

  void f5()
  {
 // Original Code: "(this->*f1('c'))(2);"
    (this->*f1('c'))(2);
  }
};

bool B::f2(int i) { return f4(f3(i)); }
bool B::f3(int i) { return 1; }

void k(A *ap)
{
  bool (A::*(A::*ff)(char))(int);
  bool (A::*pf)(int);
  ff = &A::f1;
  pf = &A::f2;
  (ap->*pf)(1);
  (ap->*(ap->*ff)('a'))(2);
  foo1();
}

void foo1()
{
  B b;
  foo1();
  k(&b);
}

int main()
{
  B b;
  k(&b);
}
