/* Only the inner loop can be parallelized
 */
void foo()
{
  int n=100, m=100;
  double b[n][m];
  int i,j;
  for (i=0;i<n;i++)
    for (j=0;j<m;j++)
      b[i][j]=b[i-1][j-1];
}
/*
dep 
SgExprStatement:(b[i])[j] =((b[i - 1])[j - 1]); 
SgExprStatement:(b[i])[j] =((b[i - 1])[j - 1]); 

2*2 TRUE_DEP; commonlevel = 2 CarryLevel = 0  Is precise 
SgPntrArrRefExp:(b[i])[j]@8:14->SgPntrArrRefExp:((b[i - 1])[j - 1])@8:21 
== -1; * 0;  ||
* 0  ; == -1; ||::
*/
