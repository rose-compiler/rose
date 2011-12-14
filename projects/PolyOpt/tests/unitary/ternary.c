int main()
{
  int i, n, b, c, d, a;
#pragma scop
  for (i = 0; i < n; ++i)
    b = c < d ? a + 1 : 2 * b; 
#pragma endscop
}
