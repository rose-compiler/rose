#ifndef TEST4_H
#define TEST4_H

// non-virtual method non_virutal_foo_XX and non-virtual destructor
class A2 {
public:
  int nonvirtual_foo();
};

class B2 : public A2 {
public:
  int nonvirtual_foo();
};

#endif
