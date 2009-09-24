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

