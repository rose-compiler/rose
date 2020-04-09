#include <cstdio>

int main()
{
  int a;
  int b = -a; // CODETHORN-84

  b=a-a; // make output independent of uninitialized value
  b=b- - (-b+10); // some additional variations
  printf("%d\n",b);
}
