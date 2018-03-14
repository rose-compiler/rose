// an example of output dependence preventing parallelization
// two level loops, check carry level value
#include <stdio.h> 
#include "omp.h" 

void foo()
{
  int i;
  int j;
  int x;
  int y;
  
#pragma omp parallel for private (y,i,j)
  for (i = 0; i <= 99; i += 1) {
    
#pragma omp parallel for private (y,j) lastprivate (x)
    for (j = 0; j <= 99; j += 1) {
      x = i;
      y = x;
      y = i + j;
      y = y + 1;
    }
  }
  printf("x=%d",x);
}
/*
 
 -------------Dump the dependence graph for the first loop in a function body!------------
dep SgExprStatement:x = i; SgExprStatement:x = i; 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:x@11:8->SgVarRefExp:x@11:8 == 0;* 0;||* 0;== 0;||::
dep SgExprStatement:x = i; SgExprStatement:y = x; 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:x@11:8[write]->SgVarRefExp:x@12:11[read] == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y = x; SgExprStatement:y = x; 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@12:9->SgVarRefExp:y@12:9 == 0;* 0;||* 0;== 0;||::
//   y =x ; x = i
 dep SgExprStatement:y = x; SgExprStatement:x = i; 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:x@12:11->SgVarRefExp:x@11:8 == 0;* 0;||* 0;<= -1;||::
 dep SgExprStatement:y = x; SgExprStatement:y =(i + j); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@12:9->SgVarRefExp:y@13:8 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y = x; SgExprStatement:y =(y + 1); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@12:9->SgVarRefExp:y@14:8 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y = x; SgExprStatement:y =(y + 1); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@12:9->SgVarRefExp:y@14:10 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y =(i + j); SgExprStatement:y =(i + j); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@13:8->SgVarRefExp:y@13:8 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y =(i + j); SgExprStatement:y = x; 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:y@13:8->SgVarRefExp:y@12:9 == 0;* 0;||* 0;<= -1;||::
 dep SgExprStatement:y =(i + j); SgExprStatement:y =(y + 1); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@13:8->SgVarRefExp:y@14:8 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y =(i + j); SgExprStatement:y =(y + 1); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@13:8->SgVarRefExp:y@14:10 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y =(y + 1); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@14:8->SgVarRefExp:y@14:8 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y =(y + 1); 2*2 SCALAR_DEP; commonlevel = 2 CarryLevel = 2 SgVarRefExp:y@14:10->SgVarRefExp:y@14:8 == 0;* 0;||* 0;== 0;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y =(y + 1); 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:y@14:8->SgVarRefExp:y@14:10 == 0;* 0;||* 0;<= -1;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y =(i + j); 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:y@14:8->SgVarRefExp:y@13:8 == 0;* 0;||* 0;<= -1;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y =(i + j); 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:y@14:10->SgVarRefExp:y@13:8 == 0;* 0;||* 0;<= -1;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y = x; 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:y@14:8->SgVarRefExp:y@12:9 == 0;* 0;||* 0;<= -1;||::
 dep SgExprStatement:y =(y + 1); SgExprStatement:y = x; 2*2 SCALAR_BACK_DEP; commonlevel = 2 CarryLevel = 1 SgVarRefExp:y@14:10->SgVarRefExp:y@12:9 == 0;* 0;||* 0;<= -1;||::
 */
