#include <math.h>

int main()
{
  int i;
  int m;
  int n;
  int p;
  int j;
  int a;
  
#pragma scop
  for (i = max(0,n); i < min((min(m,n)),p); ++i) {
  }
  for (j = max(i,n); j <= min(m,n); ++j) {
    a = 0;
  }
  
#pragma endscop
}
