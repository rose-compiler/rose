#include <math.h>

int main()
{
  int i;
  int m;
  int n;
  int p;
  int q;
  int a;
  
#pragma scop
  for (i = max((max(m,n)),(max(p,q))); i < n; ++i) {
    a = 0;
  }
  
#pragma endscop
}
