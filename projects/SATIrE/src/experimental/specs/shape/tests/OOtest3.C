#include "OOtest3.h"

// Class A implementations

// initialization of static variable
unsigned int A::objTotalNum=0;

A::A():objNum(objTotalNum++) {
  PRINTLN2("constructor A",objNum);
}

A::~A() {
  PRINTLN2("destructor A",objNum);
}


// Class B implementations
B::B() {
  PRINTLN2("constructor B",objNum);
}

int B::foo() {
  PRINTLN("B::foo called");
   return 2;
}

B::~B() {
  PRINTLN2("destructor B",objNum);
}


// Class C implementations
C::C() {
  PRINTLN2("constructor C",objNum);
}

int C::foo() {
  PRINTLN("C::foo called");
  return 3;
}

C::~C() {
  PRINTLN2("destructor C",objNum);
}


// Class D implementations
// no explicit constructor (default constructor is used)
int D::foo() {
  PRINTLN("D::foo called");
  return 3;
}

// Class X, Y implementations
// are povided in header file for testing

// Class C2 implementations
// using an explicit constructor call
// ONLY the constructor of base class B is allowed here, which is also the default behaviour
// and therefore it is superfluous to specify it explicitely 
// (in the past g++ allowed arbitrary constr.; this seems to be a fix to comply with the c++ standard)
C2::C2():B() {
  PRINTLN2("constructor C2",objNum);
}

C2::~C2() {
  PRINTLN2("destructor C2",objNum);
}


int main() {
  A* ab=new B();
  MARKER(1);
  B* b=new B();
  MARKER(2);
  // uses default constructor (which calls the constructor of base class C)
  D* d=new D();
  MARKER(3);
  ab->foo();
  b->foo();
  d->foo();
  MARKER(4);

  // use different order for destruction than construction for testing
  delete d;
  PRINTLN("Note that destructor of B on object 0 is not called here because the destructor is non-virtual!");
  delete ab;
  delete b;

  MARKER(5);
  {
    B b;
    MARKER(6);
    C c;
    MARKER(7);
    D d;
    MARKER(8);
    // destructors must be called in reverse order of constructors
  }
  MARKER(9);
  Y* y=new Y();
  MARKER(10);
  // both destructors are correct, virtual and non-virtual
  delete y;
  MARKER(11);
  X* x=new Y();
  MARKER(12);
  // uses virtual destructor (if the destructor would be non-virtual the destructor of Y would not be called)
  delete x;

  MARKER(13);
  C2* c2=new C2();
  MARKER(14);
  delete c2;
}
