// an example of output dependence preventing parallelization
// x: not live-in, yes live-out
//    outer scope
//    loop-carried output-dependence: x=... : accept values based on loop variable; or not. 
//Solution: Can be parallelized using lastprivate(x)
#include <stdio.h> 
void foo()
{
  int i,x;

  for (i=0;i<100;i++) 
    x=i;
  printf("x=%d",x);    
}  
/*  
 * output dependence, loop carried
 * 1*1  SCALAR_DEP DATA_DEP; commonlevel = 1 CarryLevel = 0 Scalar dep type  OUTPUT_DEP DATA_DEP;SgVarRefExp:x@12:6->SgVarRefExp:x@12:6 == 0;||::
 */

// This x should be not lastprivate since it is live-in
// x is both live-in and live-out, and written, cannot be reduction
// So, the loop cannot be parallelized
void foo2()
{
  int a[100];
  int i,x=10;

  for (i=0;i<100;i++) 
  { 
    a[i] = x;
    x=i;
  }
  printf("x=%d",x);    
} 
