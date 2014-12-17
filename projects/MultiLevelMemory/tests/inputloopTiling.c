#define N 100
int i,j,k;
double a[N][N],b[N][N];

int main()
{
#pragma 25
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
        a[i][j]= b[i][j];
  return 0;
}

