int a[100],b[100];
int i,x=1,j=9,k;
int main(void)
{
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

