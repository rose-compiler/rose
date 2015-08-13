// test C kernels with function calls like abs()
int abs(int val);
#define MSIZE 500
int n;
int chiterations =0, chloads=0, chstores=0, chflops=0;
double u[MSIZE],uold[MSIZE];
void dummy()
{
  int i;
  chiterations = n;
  for(i=0;i<n;i++)   
    { 
      uold[i] = u[i] + abs(0); 
    } 
}
