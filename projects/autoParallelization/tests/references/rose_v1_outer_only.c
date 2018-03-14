/* Only the outmost loop can be parallelized
 */
#include "omp.h" 

void foo()
{
  int n = 100;
  int m = 100;
  double b[n][m];
  int i;
  int j;
  
#pragma omp parallel for private (i,j) firstprivate (n,m)
  for (i = 0; i <= n - 1; i += 1) {
    for (j = 0; j <= m - 1; j += 1) {
      b[i][j] = b[i][j - 1];
    }
  }
}
/*
Unparallelizable loop at line:9 due to the following dependencies:
1*1  TRUE_DEP DATA_DEP; commonlevel = 1 CarryLevel = 0  Is precise SgPntrArrRefExp:(b[i])[j]@10:14->SgPntrArrRefExp:((b[i])[j - 1])@10:19 == -1;||::
*/
