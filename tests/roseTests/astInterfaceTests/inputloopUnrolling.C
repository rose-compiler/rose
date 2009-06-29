int a[100][100];
int main(void)
{
  int j;
  for (int i=0;i<100;i++)
    for (j=0;j<100;j++)
    {
      int k=3;
      a[i][j]=i+j+k;
    }
  return 0;
}

