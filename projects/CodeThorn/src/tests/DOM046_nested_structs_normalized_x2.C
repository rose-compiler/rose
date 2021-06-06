// CT-79
#include <stdio.h>

struct T 
{
  int tm1;
// 0
  char tm2;
// 4
}
;

struct S 
{
  int sm1;
  struct T sm2;
// 4
  char sm3;
// 9
}
;
struct S var;

int main()
{
  int x = 1;
//x=2;
  printf("x:%d\n",x);
  var . sm1 = 10;
  int &__tmp8__ = var . sm1;
  int &__tmp1__ = __tmp8__;
  printf("var.sm1:%d\n",__tmp1__);
  var . sm3 = 'A';
  char &__tmp9__ = var . sm3;
  char &__tmp2__ = __tmp9__;
  int __tmp10__ = (int )__tmp2__;
  int __tmp3__ = __tmp10__;
  printf("var.sm3:%d\n",__tmp3__);
  struct T &__tmp11__ = var . sm2;
  struct T &__tmp4__ = __tmp11__;
  __tmp4__ . tm2 = 'B';
  struct T &__tmp12__ = var . sm2;
  struct T &__tmp5__ = __tmp12__;
  char &__tmp13__ = __tmp5__ . tm2;
  char &__tmp6__ = __tmp13__;
  int __tmp14__ = (int )__tmp6__;
  int __tmp7__ = __tmp14__;
  printf("var.sm2.tm2:%d\n",__tmp7__);
}
