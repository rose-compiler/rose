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
