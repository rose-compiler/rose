int main()
{
  int k, N, alpha, sum;
#pragma scop
for (k=1; k < N; k++)
  {
    alpha = -sum;
  }
#pragma endscop
}
