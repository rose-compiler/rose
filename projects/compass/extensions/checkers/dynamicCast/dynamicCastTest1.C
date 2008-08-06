class A
{
  public:
    ~A(){}
    virtual void foo(){}
};

class B : public A
{
};

int main()
{
  A *p = new B;
  B *p2 = (B*) p;

  return 0;
}
