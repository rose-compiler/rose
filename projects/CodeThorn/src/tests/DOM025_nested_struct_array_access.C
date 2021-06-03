// CT-79

#include <stdio.h>

struct S2 {
  char member2_1[4];
  int member2_2;
};
struct S1 {
  struct S2 member1_1;
  int member1_2;
};
struct S1 global1;

int main() {
  int x=1;
  //x=2;
  printf("1 x:%d\n",x);
  global1.member1_2=10;
  printf("2 global1.member1_2:%d\n",global1.member1_2);
  // assignment NORM(LHS)=NUMBER
  global1.member1_1.member2_1[2]=2;
  printf("3 global1.member1_1.member2_1[2]:%d\n",global1.member1_1.member2_1[2]);
  // assignment VAR=NORM(RHS)
  printf("4 x:%d\n",x);
  x=global1.member1_1.member2_1[2];
  // expression: NORM(FUNCTION-ARG)
  printf("5 x:%d\n",x);
}
