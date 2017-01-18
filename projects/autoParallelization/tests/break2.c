int i,j;
int a[100][100];
void foo()
{
  for (i=0;i<100;i++)
  {
    for (j=0;j<100;j++)
    {
      a[i][j]=a[i][j]+1;
      if (a[i][j]==100)
        break;
    }
  }
}

