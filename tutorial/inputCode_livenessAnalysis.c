int a[100];

void foo2()
{
  int i;
  int tmp;
  tmp = 10;
  for (i=0;i<100;i++)
  {
    a[i] = tmp;
    tmp =a[i]+i;
  }
   a[0] = 1 ;
}
