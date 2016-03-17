/*
 * Outer loop: no dependence: 
 * Inner loop: loop-carried dependence
 *
 * final dependence graph:
 * dep SgExprStatement:(a[i])[j] =(((a[i])[j - 1]) + 1); 
 *      SgExprStatement:(a[i])[j] =(((a[i])[j - 1]) + 1); 
 * 2*2TRUE_DEP; commonlevel = 2  +precise CarryLevel = 1
 *     SgPntrArrRefExp:(a[i])[j]
 *     SgPntrArrRefExp:((a[i])[j - 1])  == 0;* 0;||* 0;== -1;||::
 */
int i;
int j;
int a[100][100];

void foo()
{
  
#pragma omp parallel for private (i,j)
  for (i = 1; i <= 99; i += 1) {
    for (j = 1; j <= 99; j += 1) {
      a[i][j] = a[i][j - 1] + 1;
    }
  }
}
