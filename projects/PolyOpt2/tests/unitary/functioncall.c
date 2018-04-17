#include <stdio.h>

int main()
{
  int i, n;
  int a[n];
  int b[n];
#pragma scop
  for (i = 0; i < n; ++i)
    printf ("%d %d", a[i+1],b[n]);
#pragma endscop
}
