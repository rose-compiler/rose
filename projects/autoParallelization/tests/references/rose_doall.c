#include "omp.h" 

void foo()
{
  int i;
  int a[100];
  
#pragma omp parallel for private (i)
  for (i = 0; i <= 99; i += 1) {
    a[i] = a[i] + 1;
  }
}
/*  
 *  ..= a[i]+1 // read
 *  a[i] == // write
 *  non-loop carried anti-dependence : Write-after-Read
 *
 dep SgExprStatement:a[i] =((a[i]) + 1); 
     SgExprStatement:a[i] =((a[i]) + 1); 
     1*1 ANTI_DEP; commonlevel = 1 CarryLevel = 1  Is precise 
  SgPntrArrRefExp:(a[i])@5:11->SgPntrArrRefExp:a[i]@5:9 == 0;||::
 
  dependence edge: 1*1 ANTI_DEP; commonlevel = 1 CarryLevel = 1  Is precise 
distance matrix: common loops surrounding two statement: 
* common level: how many levels are shared loops?
* carry level: loop dependence carry level: start from 0
  SgPntrArrRefExp:(a[i])@5:11->SgPntrArrRefExp:a[i]@5:9 == 0;||::
 *
 */
