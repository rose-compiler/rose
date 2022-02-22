#include <stdio.h>

int cnt=100;

void f1();
void f2();

void f1() {
  cnt+=1;
  printf("%d\n",cnt);
  f2();
}

void f2() {
  cnt+=10;
  printf("%d\n",cnt);
}

int main() {
  void(*fp1)()=f1;
  void(*fp2)()=f2;
  (*fp1)();
  (*fp2)();
}

  
  
