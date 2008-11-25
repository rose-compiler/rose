/*
 * The impact of loop bounds
 */
void foo(void)   
{
  int i,j;
  int a[10];
  for(i=5;i< 10;i++) 
//  for(i=0;i< 10;i++) 
  {  
   // a[i]=a[i-10]+i;
    a[i]=a[i-5]+i;
  }   
}
