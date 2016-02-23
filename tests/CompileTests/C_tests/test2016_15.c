void foo()
{
  int n=100, m=100;
  double b[n][m]; 
  int i,j,index, zoneset[m];
  for (i=0;i<n;i++)
    for (j=0;j<m;j++)
    {
      index = zoneset[j];
      b[i][index]=b[i-1][index -1];
    }
}
