#include "omp.h" 

int main(int argc,char *argv[])
{
  int i;
  int j;
  double a[20][20];
// memset(a,0,(sizeof(a)));
  for (i = 0; i <= 18; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= 19; j += 1) {
      a[i][j] += a[i + 1][j];
    }
  }
  return 0;
}
