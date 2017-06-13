int main()
{
  int i, N;
  int A[N];
  int B[N];
#pragma scop
  for (i = 0; i < N; ++i)
    A[i] = B[2 * (i) + 1];
#pragma endscop
}
