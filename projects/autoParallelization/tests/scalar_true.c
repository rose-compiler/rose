/* 
 * Scalar-to-scalar dependencies
 * */
int a[100];

void foo2()
{
  int i;
  int tmp;
  for (i=0;i<100;i++)
  {
    tmp =a[i]+i;
    a[i] = tmp;
  }
}

void foo()
{
  int i;
  int tmp;
  for (i=0;i<100;i++)
  {
    tmp =a[i]+i;
    a [i ] =tmp;
  }
  i = tmp; 
}

