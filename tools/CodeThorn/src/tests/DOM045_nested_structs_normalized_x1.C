// CT-79
#include <stdio.h>

struct T 
{
  int tm1;  // 0
  char tm2; // 4
}
;

struct S 
{
  int sm1;
  struct T sm2; // 4
  char sm3; // 9
}
;
struct S var;

int main()
{
  int x = 1;
//x=2;
  printf("x:%d\n",x);
  var . sm1 = 10;
  int &__tmp1__ = var . sm1;
  printf("var.sm1:%d\n",__tmp1__);
  var . sm3 = 'A';
  char &__tmp2__ = var . sm3;
  int __tmp3__ = (int )__tmp2__;
  printf("var.sm3:%d\n",__tmp3__);
  struct T &__tmp4__ = var . sm2;
  __tmp4__ . tm2 = 'B';
  struct T &__tmp5__ = var . sm2;
  char &__tmp6__ = __tmp5__ . tm2;
  int __tmp7__ = (int )__tmp6__;
  printf("var.sm2.tm2:%d\n",__tmp7__);
}
