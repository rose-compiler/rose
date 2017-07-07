#include <math.h>

int main()
{
  int i;
  int m;
  int n;
  int a;
  
#pragma scop
  for (i = max(m,n); i < n; ++i) {
    a = 0;
  }
  
#pragma endscop
}
