// An example of loop carried true-dependence at both levels

void foo()
{
  int n = 100;
  int m = 100;
  double a[n][m];
  int i;
  int j;
  for (i = 1; i <= n - 1; i += 1) {
    for (j = 1; j <= m - 1; j += 1) {
      a[i][j] = a[i][j - 1] + a[i - 1][j];
    }
  }
}
/*
dep SgExprStatement:(a[i])[j] =(((a[i])[j - 1]) +((a[i - 1])[j])); 
    SgExprStatement:(a[i])[j] =(((a[i])[j - 1]) +((a[i - 1])[j])); 
    2*2 TRUE_DEP; commonlevel = 2 CarryLevel = 1  
    Is precise 
    SgPntrArrRefExp:(a[i])[j]@9:14->SgPntrArrRefExp:((a[i])[j - 1])@9:19 
     == 0; *   0;||
     *  0; == -1;||::
dep SgExprStatement:(a[i])[j] =(((a[i])[j - 1]) +((a[i - 1])[j])); 
    SgExprStatement:(a[i])[j] =(((a[i])[j - 1]) +((a[i - 1])[j])); 
    2*2 TRUE_DEP; commonlevel = 2 CarryLevel = 0  
    Is precise 
    SgPntrArrRefExp:(a[i])[j]@9:14->SgPntrArrRefExp:((a[i - 1])[j])@9:31 
    == -1;  * 0;||
    *  0 ; == 0;||::
*/
