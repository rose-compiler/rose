/*
 * private(including lastprivate) scalars can be recognized by liveness analysis
 * They are dead (not belong to live-in variable sets) with respect to the loop body
 * If they are live-out with respect to the loop, it is lastprivate. 
 */
int g;
void foo()
{
  int i,x;
  int a[100];
  int b[100];
// x should be recognized as a private variable during parallelization
// yet it introduces a set of dependencies which can be eliminated
  for (i=0;i<100;i++) 
  {
   int y=i+1; 
//   g = y;
   x= a[i]+g;
    //b[i]=x+1+y;
  }  
}  
/*
 * 
dep SgExprStatement:x =(a[i]); SgExprStatement:x =(a[i]); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 0 Scalar dep type OUTPUT_DEP;SgVarRefExp:x@13:6->SgVarRefExp:x@13:6 == 0;||::

dep SgExprStatement:x =(a[i]); SgExprStatement:b[i] =(x + 1); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 Scalar dep type TRUE_DEP;SgVarRefExp:x@13:6->SgVarRefExp:x@14:10 == 0;||::

dep SgExprStatement:b[i] =(x + 1); SgExprStatement:x =(a[i]); 1*1 SCALAR_BACK_DEP; commonlevel = 1 CarryLevel = 0 Scalar dep type ANTI_DEP;SgVarRefExp:x@14:10->SgVarRefExp:x@13:6 <= -1;||::
 */
