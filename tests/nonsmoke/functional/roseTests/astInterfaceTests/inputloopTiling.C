#define N 100
int i,j,k;
double a[N][N],b[N][N],c[N][N];

int main()
{
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
        c[i][j]= c[i][j]+a[i][k]*b[k][j];
  return 0;
}

