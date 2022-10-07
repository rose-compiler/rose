#include <cstdlib>

struct S {
  int x;
  int y;
};

int main() {
  S s1;
  int a;
  S* p;
  p=&s1;
  p->x=5;
  p->y=6;
  a=p->x;
  a=a+p->x+p->y;
}
