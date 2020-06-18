#include <cstdio>

int main() {
  int a=1;
  int b=10;
  int* pa=&a;
  int& ra=a;
  int& rb=b;
  printf("a:%d, b:%d, ra:%d, rb:%d\n",a,b,ra,rb);
  ra=ra+2;
  printf("a:%d, b:%d, ra:%d, rb:%d\n",a,b,ra,rb);
  ra=rb;
  printf("a:%d, b:%d, ra:%d, rb:%d\n",a,b,ra,rb);
  ra=ra+4;
  printf("a:%d, b:%d, ra:%d, rb:%d\n",a,b,ra,rb);
}
