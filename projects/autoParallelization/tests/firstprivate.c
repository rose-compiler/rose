/*
 * dependence graph:
 */
int g;
void foo()
{
  int i,x,y=1;
  int a[100];
  int b[100];
// x and g should be recognized as a firstprivate variable during parallelization
// Does it introduce any data dependencies ?
// could be (as regular private variables )
  for (i=0;i<100;i++) 
  { 
    y=x+1+g;
    //b[i]=x+1+g;
    // x=...
    // ... =x
  }  
  x=1;
}  
/*
 
 */
