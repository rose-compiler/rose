#include <math.h>

int main()
{
  int i, m, n, p, q, o, a;
#pragma scop
  for (i = 0; i < min(min(m,q),min(n,min(p,o))); ++i)
    a = 0;
#pragma endscop
}
