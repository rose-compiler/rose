#define N 100
int i,j,k;
double a[1][1][N][N],b[1][1][N][N];

int main()
{
#pragma 25
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
    {
        a[0][0][i][j]= b[0][0][i][j];
    }
  return 0;
}

