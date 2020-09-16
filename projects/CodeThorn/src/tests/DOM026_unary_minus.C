#include <cstdio>

int main()
{
  int a;
  int b = -a; // CODETHORN-84
  a=1;
  b=-a;
  b=b- - (-b+10); // some additional variations
  printf("%d\n",b);
}
