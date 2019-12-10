#define n 100

  double a[n], b[n], c[n], d[n], e[n];
  double tot[n][ n];
  double dux[n][n][n], duy[n][n][n], duz[n][n][n];

main() 
{
  int i,j,k;
      for ( j=0; j <=n-1; j+=1)
      for ( i=0; i<=n-1; i+=1)
         duz[i][j][0] = duz[i][j][0]*b[0];

      for (k=1; k<=n-2; k+=1)
      for (j = 0; j <= n-1; j+=1)
      for (i=0; i<=n-1; i+=1)
         duz[i][j][k]=(duz[i][j][k]-a[k]*duz[i][j][k-1])*b[k];

      for ( j=0; j <=n-1; j+=1)
      for ( i=0; i<=n-1; i+=1)
         tot[i][j] = 0;

      for ( k=0; k<=n-2; k+=1)
      for ( j=0; j <=n-1; j+=1)
      for ( i=0; i<=n-1; i+=1)
            tot[i][j] = tot[i][j] + d[k]*duz[i][j][k];

      for ( j=0; j <=n-1; j+=1)
      for ( i=0; i<=n-1; i+=1)
         duz[i][j][n-1] = (duz[i][j][n-1] - tot[i][j])*b[n-1];

      for ( j=0; j <=n-1; j+=1)
      for ( i=0; i<=n-1; i+=1)
         duz[i][j][n-2]=duz[i][j][n-2] - e[n-2]*duz[i][j][n-1];

      for (k=n-3; k>=0; k+=-1) 
      for (j = 0; j <= n-1; j+=1)
      for (i=0; i<=n-1; i+=1)
         duz[i][j][k] = duz[i][j][k] - c[k]*duz[i][j][k+1] - e[k]*duz[i][j][n-1];
      
}
