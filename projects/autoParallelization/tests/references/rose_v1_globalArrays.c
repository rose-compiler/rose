#define MSIZE 256
#include "omp.h" 
double u[256][256];
double f[256][256];
int n;
int m;

void initialize()
{
  int i;
  int j;
  int xx;
  n = 256;
  m = 256;
  double dx = 2.0 / (n - 1);
  
#pragma omp parallel for private (xx,i,j) firstprivate (n,m)
  for (i = 0; i <= n - 1; i += 1) {
    
#pragma omp parallel for private (xx,j) firstprivate (dx)
    for (j = 0; j <= m - 1; j += 1) {
      xx = ((int )(- 1.0 + dx * (i - 1)));
      u[i][j] = 0.0;
      f[i][j] = - 1.0 * (1.0 - (xx * xx));
    }
  }
}
