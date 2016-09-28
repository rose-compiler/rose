/*
 * An example to see if loop-carried data(true) dependence can be detected by ROSE
 *
 * Liao, 5/5/2008
 */
//#include <stdio.h>
int main(void)   
{
  int sum = 0;
  int i=1;
  for(;i<= 100;) 
  {  
    sum = sum +i;
    i = i+1;
  }   
//  printf("sum=%d\n",sum);
  return 0;
}
