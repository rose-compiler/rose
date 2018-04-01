#include <iostream>

int main() {
  float f1;
  double d1;
  int i1;
  i1=2;
  f1=2.0;
  d1=f1;
  f1=d1;
  d1=3.0;
  d1=f1;
  if(d1==f1) {
    i1=(int)f1;
  }
  d1=f1=i1;
  double testVar;
  long double ld1;
  double* pd=&d1;
  float* pf=&f1;
  *pf=0.0;
  *pd=0.0f;
  *pf=*pd;
  testVar=(long double)10.0;
  ld1=testVar;
  std::cout<<"testVar:"<<testVar<<std::endl;
  std::cout<<"ld1:"<<ld1<<std::endl;
  return 0;
}
