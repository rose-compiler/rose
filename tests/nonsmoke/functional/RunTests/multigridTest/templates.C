#include <iostream.h>

template <class X> class number
{
  X value;

public:
  void setVal(X v); 
  X getVal() { return value; }

};

template <class X> void number<X>::setVal(X v) { value = v; }

class test
{
  int value;

public:

  void setVal(int v); 
  int getVal() { return value; }
  test getThis() { return *this; }

  test operator+(test v)
  {
    test temp;
    temp.setVal(value+v.getVal());
    return temp;

  }

  void operator+=(test v)
  {
    value+=v.getVal();

  }
  
  int operator<(test v)
  {
    return (value<v.getVal());

  }

friend test operator-(test v);

};

test operator-(test u, test v)
{
  test temp;
  temp.setVal(u.getVal()-v.getVal());
  return temp;

}

void test::setVal(int v) { value = v; }

main()
{
  number<int> i;
  number<float> f;
  test t,u,v;

  int testInt = 0;

  cout << "testInt = " << testInt << endl;

  i.setVal(3);

  f.setVal(3.14);

  t.setVal(3);

  cout << "i = " << i.getVal() << endl;

  cout << "f = " << f.getVal() << endl;

  cout << "t = " << t.getVal() << endl;
 
  u = t.getThis();

  cout << "u = " << u.getVal() << endl;
 
  v = t+u;

  cout << "t+u = " << v.getVal() << endl;

  v+=u;

  cout << "v+=u = " << v.getVal() << endl;

  cout << "t<u = " << (t<u) << endl;

  cout << "max(i,f) = " << (i.getVal()>f.getVal()?i.getVal():f.getVal() ) << endl;

  return 0;

}

