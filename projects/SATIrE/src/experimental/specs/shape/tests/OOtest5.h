#ifndef TEST5_H
#define TEST5_H

template<typename T>
class AT1 {
public:
  AT1() {}
  virtual int virtual_foo()=0;
};

class AT2 : public AT1<int> {
public:
  virtual int virtual_foo() { return 200; }
};


// virtual method virtual_foo and virtual destructor
class A1 {
public:
  // user-defined default constructor
  A1() {}
  // destructor
  virtual ~A1();

  // copy constructor (also used in initialization =)
  A1(const A1& origToCopy);

  // conversion operator (FROM int TO A1)
  A1(const int x);

  // conversion operator (FROM A1 TO int)
  operator int();

  // assignment operator
  A1& operator=(const A1& a1);

  // member function
  virtual int virtual_foo();
private:
  int val;
};

class B1 : public A1 {
public:
  // no user-defined constructor
  ~B1();
  int virtual_foo();
};

// non-virtual method non_virutal_foo_XX and non-virtual destructor
class A2 {
public:
  A2();
  ~A2();
  int nonvirtual_foo();
};

class B2 : public A2 {
public:
  B2();
  ~B2();
  int nonvirtual_foo();
};

#endif
