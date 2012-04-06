/*
Naive matrix-vector multiplication
By C. Liao
*/
#define N 1000

int i,j,k;
double a[N][N],v[N],v_out[N];

int mmm()
{
  for (i = 0; i < N; i++)
  {
    float sum = 0.0;
    for (j = 0; j < N; j++)
    {
      sum += a[i][j]*v[j];
    }
    v_out[i] = sum;
  }
  return 0;
}

