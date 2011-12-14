int main()
{
  int i, n, a;
#pragma scop
  for (i = 0; i < n; ++i)
    a = 0;
#pragma endscop
}
