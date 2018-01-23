/* Only the inner loop can be parallelized
 */
#include "omp.h" 

void foo()
{
  int n = 100;
  int m = 100;
  double b[n][m];
  int i;
  int j;
  for (i = 0; i <= n - 1; i += 1) {
    
#pragma omp parallel for private (j)
    for (j = 0; j <= m - 1; j += 1) {
      b[i][j] = b[i - 1][j - 1];
    }
  }
}
