#ifndef A_H
#define A_H

class A
{
 public:
  A(void);
};

template<class ANames> class TypedA : public A
{
 public:
 TypedA(): A() {};
};

#endif
