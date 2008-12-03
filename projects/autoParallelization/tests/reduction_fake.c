/*
 *  Test for automatic recognition of reduction variables
 * */
extern int bar();
int a[100], sum;
void foo()
{
  int i,sum2,xx,yy,zz;
  sum = 0;
  for (i=0;i<100;i++)
  {
    a[i]=i;
    sum = a[i]+ sum +bar();    
//    sum = a[i]+ sum ;    
  }
  sum2=sum;
  a[1]=1;
}
