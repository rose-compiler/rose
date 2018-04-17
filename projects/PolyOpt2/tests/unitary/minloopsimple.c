#include <math.h>

int main()
{
  int i, m, n, a;
#pragma scop
for (i = 0; i < min(m,n); ++i)
  a = 0;
#pragma endscop
}
