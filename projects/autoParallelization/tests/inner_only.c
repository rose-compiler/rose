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
