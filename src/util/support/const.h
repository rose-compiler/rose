
#ifndef CONST_H
#define CONST_H

#define NEG_INFTY -32767
#define POS_INFTY 32767

inline int AddConst( int a, int b)
{
  if (a == NEG_INFTY || a == POS_INFTY) {
     return a;
  }
  if (b == NEG_INFTY || b == POS_INFTY) {
     return b;
  }
  return a + b;
}


#endif
