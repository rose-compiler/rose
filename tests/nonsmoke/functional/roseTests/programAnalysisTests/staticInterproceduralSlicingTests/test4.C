/*
 * Another example to see if loop-carried dependence
 * for array references can be detected by ROSE
 *
 * Current data dependence analysis treats an array as a whole, not 
 * individual array elements. So a[i]=a[i-50]+i is wrongfully declared to have 
 * loop-carried data dependence.
 *
 * Liao, 5/7/2008
 */
//#include <stdio.h>
int main(void)   
{
  int i,j;
  int a[100];
#if 0  
  for(i=0;i< 100;i++) 
  {  
    a[i]=i;
  }   
#else
  for(i=50;i< 100;i++) 
  {  
    a[i]=a[i-50]+i;
  }   
#endif
//  printf("sum=%d\n",sum);
  return 0;
}
