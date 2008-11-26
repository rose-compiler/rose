/*
 * The impact of loop bounds
 *
 * a[i] --> a[i-5]  s1 ->s2
 * Dependence condition: direction <= and alignment factor -5
 * I(s1) <= I (s2) -5
 *
 * Loop bounds condition:  upper - lower must >=5
 *  I(s2) - I(s1) >=5 
 */
void foo(void)   
{
  int i,j;
  int a[10];
  for(i=5;i<9;i++) 
//  for(i=0;i< 10;i++) 
  {  
    a[i+5]=a[i]+i;
    //a[i]=a[i-5]+i;
  }   
}
