#ifndef TEST4_H
#define TEST4_H

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
  // no user-defined constructor
  A1() {}
  virtual ~A1();
  virtual int virtual_foo();
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
