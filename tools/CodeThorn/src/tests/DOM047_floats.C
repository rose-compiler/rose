#include <cstdio>

int main() {
  double a=5.75;
  printf("a:%f\n",a);
  float x;
  x=5.25;
  printf("x:%f\n",x);
  x=-x;
  printf("x:%f\n",x);
  double y;
  y=10.55;
  printf("y:%f\n",y);
  double z=a+x+y; // test implicit type conversion
  printf("z:%lf\n",z);
}
