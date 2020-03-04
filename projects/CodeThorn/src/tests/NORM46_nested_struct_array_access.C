// CT-79

#include <stdio.h>

struct S2 {
  char member2[4];
};
struct S1 {
  struct S2 member1;
};
struct S1 global1;

int main() {
  char x=1;
  // assignment NORM(LHS)=NUMBER
  global1.member1.member2[2]=1;
  // assignment VAR=NORM(RHS)
  x=global1.member1.member2[2];
  // expression: NORM(FUNCTION-ARG)
  printf("%d\n",global1.member1.member2[2]);
}
