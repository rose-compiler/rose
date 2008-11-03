// an example of output dependence preventing parallelization
#include <stdio.h> 
void foo()
{
  int i,x;

  for (i=0;i<100;i++) 
    x=i;
  printf("x=%d",x);    
}  
/*  carryLevel should be 0?
 
 * dep SgExprStatement:x = i; SgExprStatement:x = i; 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:x@7:6->SgVarRefExp:x@7:6 == 0;||::

 */
