
class Base 
{
public:
  Base():x(0){}
  virtual void f1()=0;
  virtual void f2()=0;
  virtual void f3();
  void f4();
  int getX() { return x; }
protected:
  int x;
};

// define pure virtual function outside class
void Base::f2 () {
  x+=256;
}

void Base::f3() { x+=4; }

void Base::f4() { x+=8; }

class Derived : public Base
{
public:
  void f1() {
    x+=16;
  }
  // C++ language corner case:
  // implements pure abstract function by using implementation in Base
  void f2() {
    Base::f2(); // call function that is implemented in base class
  }
  // implements pure virtual function
  void f3() {x+=64;}
  // shadows Base::f4
  void f4() {x+=128;}
};

int main() {
  Derived d;
  d.f1();
  d.f2();
  d.f3();
  d.f4();
  return d.getX()==464?0:-1;
}
