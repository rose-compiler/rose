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
  
// problematic case: when a.f(a.g()) is normalized into 
// A::MyInt __tmp1__=a.g(); a.f(__tmp1__);
// then A::MyInt cannot be used because it is a private type of A.
// Possible solutions:
// (1) A::typedef int MyInt is made public to be valid C++ code.
// (2) A::tmpType1 is used instead (being type equivalent)
// (3) a new public typedef is added that aliases the private type 
//     solution 3 is the same as solution 2 in this case here.
  a.f(a.g());

  //---
  A b;
  {
    A::tmpType1 tmp1=b.g();
    b.f(tmp1);
  }
  return 0;
}
