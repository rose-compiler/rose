#include <stdio.h>

int main()
{
  int n = 200;
  char c1 = 'i';
  char c2 = n;
  char c3 = 200;
  int i = 1000;

  printf( "%c/c2 = %d\n%c/c3 = %d\n", c1, i/c2, c1, i/c3);

  return 0;
}
