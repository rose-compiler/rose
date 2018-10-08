/* Only the outmost loop can be parallelized
 */
void foo()
{
  int n=100, m=100;
  double b[n][m];
  int i,j;
  for (i=0;i<n;i++)
    for (j=1;j<m;j++)
      b[i][j]=b[i][j-1];
}
/*
Unparallelizable loop at line:9 due to the following dependencies:

1*1  TRUE_DEP DATA_DEP; commonlevel = 1 CarryLevel = 0  Is precise SgPntrArrRefExp:(b[i])[j]@10:14->SgPntrArrRefExp:((b[i])[j - 1])@10:19 == -1;||::
*/
