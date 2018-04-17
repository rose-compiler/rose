#include <math.h>

int main()
{
  int i, m, n, p, q, a;
#pragma scop
for (i = max(max(m,n),max(p,q)); i < n; ++i)
  a = 0;
#pragma endscop
}
