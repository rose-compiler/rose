// Liao, 10/29/2012
// Dynamic cast to a reference type. 
// A test case to test EDG 4.x's addition of new IR kind to this situation
class A
{
  public:
    virtual ~A()
    {
    }
};

class B : public A
{
};

int  main()
{
  A* p = new A;

  //Using reference
  B& b = dynamic_cast<B&>(*p);
  return 0;
}

