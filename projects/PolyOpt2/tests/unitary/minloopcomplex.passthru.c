#include <math.h>

int main()
{
  int i;
  int m;
  int n;
  int p;
  int q;
  int o;
  int a;
  
#pragma scop
  for (i = 0; i < min((min(m,q)),(min(n,(min(p,o))))); ++i) {
    a = 0;
  }
  
#pragma endscop
}
