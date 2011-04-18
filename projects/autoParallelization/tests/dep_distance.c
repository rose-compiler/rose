/*
 *Test dependence distance 
 * */
void foo()
{
  int i;
  int a[100];
/* Constant offset*/
  for (i=0;i<99;i++)
    a[i+3]=a[i-5]+1;
}

void foo2(int j, int k)
{
  int i;
  int a[100];

/*variable offset*/
  for (i=0;i<99;i++)
    a[i+j]=a[i+k]+1;
}

int b[100][100];
void foo3()
{
  int i,j;
/*two level with constant offset*/
  for (i=1;i<100;i++)
    for (j=1;j<100;j++)
      b[i][j]=b[i][j-1]+1;
}

