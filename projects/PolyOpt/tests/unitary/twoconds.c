int main()
{
  int M, N, a, b, i;
#pragma scop
  for (i = 0; i < N; ++i)
    {
      if (M > N)
	a;
      if (M <= N)
	b;
    }
#pragma endscop
}
