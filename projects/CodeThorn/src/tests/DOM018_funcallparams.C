#include <cstdio>
int f(int* p) {
  *p=*p+1;
  p++;
  *p=*p+1;
}
int g(int* p) {
  *p=*p+1;
  p++;
  *p=*p+1;
}

int main() {
  int a[]={1,2,3};
  f(a);
  g(a+1);
  printf("%d",a[0]);
  printf("%d",a[1]);
  printf("%d",a[2]);
}
