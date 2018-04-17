int main()
{
  int i, n, a;
#pragma scop
  for (i = 0; i < n; i = i + 1)
    a = 0;
#pragma endscop
}
