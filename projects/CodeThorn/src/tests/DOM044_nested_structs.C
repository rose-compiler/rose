// CT-79

#include <stdio.h>

struct T {
  int tm1;
  char tm2;
};
struct S {
  int sm1;
  struct T sm2;
  char sm3;
};

struct S var;

int main() {
  int x=1;
  //x=2;
  printf("x:%d\n",x);
  var.sm1=10;
  printf("var.sm1:%d\n",var.sm1);
  var.sm3='A';
  printf("var.sm3:%d\n",var.sm3);
  var.sm2.tm2='B';
  printf("var.sm2.tm2:%d\n",var.sm2.tm2);
}
