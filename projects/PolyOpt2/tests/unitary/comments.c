int main()
{
  int i, n;
  int c[n];
#pragma scop
  /* A comment in the scop. */
  for (i=1;i<=n;i++)
    // A C++ comment.
    c[i] = 0;
  /* Another comment in the scop. */
#pragma endscop
}
