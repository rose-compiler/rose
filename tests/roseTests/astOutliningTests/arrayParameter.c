//an array parameter is implicitly converted with a pointer type for its first dimension
//Need special handling during outlining.
void foo(int sum[100][50])
{
  int i,j;
#pragma rose_outline
  for (i=0;i<100;i++)
    for (i=0;i<100;i++)
      sum[i][j] =0;
}
