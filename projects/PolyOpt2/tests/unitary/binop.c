int main()
{
  int a, b, n, i;
#pragma scop
  for (i = 0; i < n; ++i)
    {
      a += b;
      --a;
      b++;
    }
#pragma endscop
}
