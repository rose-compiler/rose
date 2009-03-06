void foo()
{
  int i,j=100, sum[100];
#pragma rose_outline
  for (i=0;i<100;i++)
  {
    sum[i] =i*2+j;
  }
  
}
