#include <cstdio>

class A {
private:
  typedef int MyInt;
public:
  typedef MyInt tmpType1; // added for normalization
  void f(MyInt x) { printf("%d\n",x); }
  MyInt g() { MyInt x=1; return x;}
};

int main() {
  A a;
  typedef long MyInt;
  a.f(a.g());
  //---
  A b;
  {
    A::tmpType1 tmp1=b.g();
    b.f(tmp1);
  }
  return 0;
}
