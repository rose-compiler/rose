#ifndef OOTEST3_H
#define OOTEST3_H

#ifdef OUTPUT
#include <iostream>
#endif

#ifdef OUTPUT
#define MARKER(x) std::cout << "<---- " << x << " ---------->" << std::endl;
#define PRINTLN(__text) std::cout << __text << std::endl;
#define PRINTLN2(__text,__objnum) std::cout << __text << " of object: " << __objnum << std::endl;
#else
#define MARKER(x) ;
#define PRINTLN(__text) ;
#define PRINTLN2(__text,__objnum) ;
#endif

class A {
public:
  A();
  virtual int foo()=0;
  ~A();
  const unsigned int objNum;
protected:
  static unsigned int objTotalNum;
  // stores number of object (for reporting construction/destruction)
};

class B : public A {
public:
  B();
  virtual int foo();
  ~B();
};

class C : public B {
public:
  C();
  ~C();
  virtual int foo();
};

class D : public C {
public:
  virtual int foo();
};

// For testing we use the implementation in the header
// file (in difference to above classes
// class X uses a pointer to class D
// the constructor creates an object of class D
class X {
public:
  X() {
    PRINTLN("constructor X");
    // use call in constructor for init (instead of initializer list)
    _val1=new D();
  }
  virtual ~X() {
    PRINTLN("destructor X");
    delete _val1;
  }
private:
  D* _val1; 
};

class Y : public X {
public:
  // use initializer list (different to class X)
  // constructor for X() gets called as well
  Y():_val2(new B()) {
    PRINTLN("constructor Y");
  }
  ~Y() {
    PRINTLN("destructor Y");
    delete _val2; 
  }
private:
  B* _val2;
};

// for testing explicit constructor calls in initializer list
class C2 : public B {
public:
  C2();
  ~C2();
};


#endif
