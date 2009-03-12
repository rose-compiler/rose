int main(void)
{
  int i;
  int x=1;
  int k;
  int a[100];
  int b[100];
  int j=9;
  for (i=0;i<100;i+=1)
  {
    a[i]=i+x+j+b[i];
    k=i;
  }
  i=k;
  for (i=0;i<100;i+=1)
  {
    b[i]=i+j+a[i];
    //a[i]=i+j+k;
  }

  return 0;
}

