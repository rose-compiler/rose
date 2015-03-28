#include <cstdio>

int f()
{
  int a[2][2];
  for (unsigned i = 0; i < 2; i++)
    for (unsigned j = 0; j < 2; j++)
      a[i][j] = i*j;
  for (unsigned i = 0; i < 2; i++)
    for (unsigned j = 0; j < 2; j++)
      printf("%d ", a[i][j]);
  return 0;
}
