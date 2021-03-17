#include <cstdio>

int main() {

  struct U {
    int* a;
    int* b;
    struct S* sCycleLink;
  };
  struct T {
    int a;
    struct U u;
    int b;
  };
  struct S {
    int a;
    int b;
    struct T t;
    int c;
  };
  int x=100;
  int* p=&x;
  S s;
  s.a=1;
  s.b=2;
  s.t.a=3;
  s.t.u.a=p;
  s.t.u.b=s.t.u.a;
  s.t.u.sCycleLink=&s;
  s.c=3;
  printf("s.a:%d\n",s.a);
  printf("s.b:%d\n",s.b);
  printf("*s.t.u.a:%d\n",*s.t.u.a);
  printf("*s.t.u.b:%d\n",*s.t.u.b);
  printf("s.c:%d\n",s.c);
}
