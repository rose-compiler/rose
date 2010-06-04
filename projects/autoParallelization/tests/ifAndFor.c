//! if statement without an explicit basic block as a body
void foo(int j)
{
  int i, a[100];
  if (j!=-1)
    for (i=0;i<100;i++) 
      a[i]=a[i]+1;
}  
