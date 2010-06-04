int g;
void foo()
{
  int i,x,y=1;
  int a[100];
  int b[100];
  for (i=0;i<100;i++) 
  { 
    y=x+1+g;
    b[i]=x+1+g;
    // x=...
    // ... =x
  }  
  x=g;
}  

int a[100];
void foo2()
{
  int i;
  int tmp;
  tmp = 10;
  // It would be wrong to parallelize the following loop
  // since the true dependence between tmp in an iteration
  // and tmp in the following iteration.
  // Even firstprivate cannot help this.
  for (i=0;i<100;i++)
  {
    a[i] = tmp;
    tmp =a[i]+i;
  }

  printf ("a[0]=%d\n", a[0]);
  printf ("a[40]=%d\n", a[40]);
  printf ("a[99]=%d\n", a[99]);
}

