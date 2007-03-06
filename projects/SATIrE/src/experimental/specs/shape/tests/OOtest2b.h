#ifndef TEST2_H
#define TEST2_H

class A {
public:
  A();
  ~A();
  int val;
};

class B : public A {
public:
  B();
  ~B();
};

#endif
