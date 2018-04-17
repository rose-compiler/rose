//! loop with a break
void foo()
{
  int i, a[100];
  for (i=0;i<100;i++) 
  {
    a[i]=a[i]+1;
    if (a[i]==100)
      break;
  }
}  
