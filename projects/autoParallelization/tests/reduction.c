/*
 *  Test for automatic recognition of reduction variables
 * */
int a[100], sum;
void foo()
{
  int i,sum2,xx,yy,zz;
  sum = 0;
//#pragma omp parallel for private (i),reduction (+:sum,xx),reduction (-:yy),reduction (*:zz)  
  for (i=0;i<100;i++)
  {
    a[i]=i;
    sum = a[i]+ sum;    
    xx++;
    yy--;
    zz*=a[i];
  }
  sum2=sum+xx+yy+zz;
  a[1]=1;
}
