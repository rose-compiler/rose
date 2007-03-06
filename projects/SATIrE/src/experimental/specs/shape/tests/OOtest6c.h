#ifndef TEST4_H
#define TEST4_H

// virtual method virtual_foo and virtual destructor
class A1 {
public:
  // no user-defined constructor
  virtual ~A1();
  int val;
};

class B1 : public A1 {
public:
  // no user-defined constructor
  ~B1();
};

#endif
