int main()
{
  int i, j, N;
  int b[N][N];
  int a[N][N];
#pragma scop
  for (i=2; i<N-1; i++) {
    for (j=2; j<N-1; j++) {
      b[i][j]= 0.2*(a[i][j]+a[i][j-1]+a[i][1+1]);
    }
  }
#pragma endscop
}
