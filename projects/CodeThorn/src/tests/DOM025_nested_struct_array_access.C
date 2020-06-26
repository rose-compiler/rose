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
  char x=1;
  // assignment NORM(LHS)=NUMBER
  global1.member1_1.member2_1[2]=1;
  // assignment VAR=NORM(RHS)
  x=global1.member1_1.member2_1[2];
  // expression: NORM(FUNCTION-ARG)
  printf("%d\n",global1.member1_1.member2_1[2]);
}
