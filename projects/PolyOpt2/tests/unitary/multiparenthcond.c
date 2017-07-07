int main()
{
  int M, N, a;
#pragma scop
  if (((M) > (N+1)))
    a;
#pragma endscop
}
