// 1 Matches

#include <cstring>

struct vector {
  double a;
  double b;
};

int main(){
  vector *x, *y;
  x->a = y->b = 5;
  x->b = y->a = 7;
  memcmp(x, y, sizeof(*x));
  return 0;
}
