int a[100];
int j;
int main(void)
{
  for (int i=0;i<100;i++)
    a[i]=i;

 int i;
  for (i=100;i>-1;i--)
    a[i]=i;

  for (i=100;i>=0;i-=2)
    a[i]=i;

  return 0;
}

