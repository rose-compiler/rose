int main()
{
  int i, j, n;
  int a, c;
#pragma scop
  for (i = 0; i < n; ++i)
    for (j = 0; j < 2; ++j)
      a;
  for (i = 0; i < n; ++i)
    c;  
#pragma endscop
}
