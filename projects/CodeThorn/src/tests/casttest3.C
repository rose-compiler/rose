float function1(int x, double b) {
  b+=1.0;
  long double d1;
  d1=x+b;
  return d1;
}

int main() {
  float f1;
  double d1;
  int i1;
  i1=2;
  f1=2.0;
  d1=3.0;
  d1=f1;
  if(d1==f1) {
    i1=(int)f1;
  }
  f1=function1(i1,d1);
  d1=f1=i1;
  double testVar;
  long double ld1;
  testVar=(long double)10.0;
  ld1=testVar;
  return ld1==testVar;
}
