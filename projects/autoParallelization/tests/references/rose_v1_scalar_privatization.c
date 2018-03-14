/* a local variable to transfer temp value
 * It introduces "fake" data dependence since the variable is local to each iteration
 * */
#include "omp.h" 
int a[100];
int b[100];

void foo()
{
  int i;
  
#pragma omp parallel for private (i)
  for (i = 0; i <= 99; i += 1) {
    int tmp;
    tmp = a[i] + i;
    b[i] = tmp;
  }
}
/*
 *-------------Dump the dependence graph for the first loop in a function body!------------
// Output dependence
// Loop-carried ,why CarryLevel =1????
dep SgExprStatement:tmp =((a[i]) + i); SgExprStatement:tmp =((a[i]) + i); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:tmp@11:9->SgVarRefExp:tmp@11:9 == 0;||::
//True dependence for both 
//a) loop independent (within an iteration) and
//b) loop carried (across iterations) : This is sure thing if a) holds
dep SgExprStatement:tmp =((a[i]) + i); SgExprStatement:b[i] = tmp; 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:tmp@11:9->SgVarRefExp:tmp@12:12 == 0;||::
//Anti dependence
//Loop carried(BACK_DEP)  scalar
dep SgExprStatement:b[i] = tmp; SgExprStatement:tmp =((a[i]) + i); 1*1 SCALAR_BACK_DEP; commonlevel = 1 CarryLevel = 0 SgVarRefExp:tmp@12:12->SgVarRefExp:tmp@11:9 <= -1;||::
 */
