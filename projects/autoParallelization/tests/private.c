/*
 * dependence graph:
 */
void foo()
{
  int i,x;
  int a[100];
  int b[100];
// x should be recognized as a private variable during parallelization
// yet it introduces a set of dependencies 
  for (i=0;i<100;i++) 
  { 
    x= a[i];
    b[i]=x+1;
  }  
}  
/*
 * 
output dependence:  should be loop carried (level=0), but reported as non-loop carried?
dep SgExprStatement:x =(a[i]); SgExprStatement:x =(a[i]); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:x@13:6->SgVarRefExp:x@13:6 == 0;||::

true dependence: non-loop carried
dep SgExprStatement:x =(a[i]); SgExprStatement:b[i] =(x + 1); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:x@13:6->SgVarRefExp:x@14:10 == 0;||::

anti-dependence: loop -carried
dep SgExprStatement:b[i] =(x + 1); SgExprStatement:x =(a[i]); 1*1 SCALAR_BACK_DEP; commonlevel = 1 CarryLevel = 0 SgVarRefExp:x@14:10->SgVarRefExp:x@13:6 <= -1;||::

 */
