// Hi Dan, Pei-Hung
//    The following simple c++14 code snippet fails to compile with ROSE configured with EDG 4.12

#include <iostream>

auto add(int x, int y) { return x+y; }
int main(){
  const int a = add(2,3);
  return 1;
}

// I compiled it as follows: identityTranslator -std=c++14 test.cc

