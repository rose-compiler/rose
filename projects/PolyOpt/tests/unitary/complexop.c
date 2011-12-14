int main()
{
  int a, b, c, n, i;
  int d[n];
#pragma scop
  for (i = 0; i < n; ++i)
    a ^= (b | (c + 2)) % 2 - !(d[n]/2);
#pragma endscop
}
