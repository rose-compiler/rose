#include <cstdio>

int main() {

  struct X {
    int a;
    int b;
  };
  X s={1,2};
  printf("s.a:%d\n",s.a);
  printf("s.b:%d\n",s.b);
  X& sref=s;
  int x;
  x=sref.a;
  printf("x:%d\n",x);
}
