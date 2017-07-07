int main()
{
  int i, M, N, j, k, K;
  int C[N][N];
  int _C_[N][N];
  int A[N][N];
  int B[N][N];
#pragma scop
  for(i=0; i<M; i++)
    for(j=0; j<N; j++)
      for(k=0; k<K; k++)
	C[i][j] = _C_[i][j] + A[i][k] * B[k][j];
#pragma endscop
}
