// CT-79

#include <stdio.h>

struct T {
  char tm1[4];
  int tm2;
};
struct S {
  struct T sm1;
  int sm2;
};

struct S var;

int main() {
  int x=1;
  //x=2;
  printf("1 x:%d\n",x);
  var.sm2=10;
  printf("2 var.sm2:%d\n",var.sm2);
  var.sm1.tm2=100;
  printf("3 var.sm1.tm2:%d\n",var.sm1.tm2);
  // assignment NORM(LHS)=NUMBER
  var.sm1.tm1[2]=2;
  printf("4 var.sm1.tm1[2]:%d\n",var.sm1.tm1[2]);
  // assignment VAR=NORM(RHS)
  printf("5 x:%d\n",x);
  x=var.sm1.tm1[2];
  // expression: NORM(FUNCTION-ARG)
  printf("6 x:%d\n",x);
}
