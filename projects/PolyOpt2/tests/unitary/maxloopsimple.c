#include <math.h>

int main()
{
  int i, m, n, a;
#pragma scop
  for (i = max(m,n); i < n; ++i)
    a = 0;
#pragma endscop
}
