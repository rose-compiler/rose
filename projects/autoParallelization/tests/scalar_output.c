/* 
 * Scalar-to-scalar output dependencies
 * */
int a[100];

// A private case
void foo2()
{
  int i;
  int tmp;
  for (i=0;i<100;i++)
  {
    tmp =a[i]+i;
  }
}

// A lastprivate case
void foo()
{
  int i;
  int tmp;
  for (i=0;i<100;i++)
  {
    tmp =a[i]+i;
  }
  i = tmp; 
}


