
#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE

# define _PB_TSTEPS STEPSIZE 
# define _PB_N ARRAYSIZE 


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  double A[N][N];
  double B[N][N];
  int t, i, j;

#pragma scop
  for (t = 0; t < _PB_TSTEPS; t++)
    {
      for (i = 1; i < _PB_N - 1; i++)
	for (j = 1; j < _PB_N - 1; j++)
	  B[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i][1+j] + A[1+i][j] + A[i-1][j]);
      for (i = 1; i < _PB_N-1; i++)
	for (j = 1; j < _PB_N-1; j++)
	  A[i][j] = B[i][j];
    }
#pragma endscop

  return 0;
}
