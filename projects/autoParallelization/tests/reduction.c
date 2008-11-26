/*
 * liveness for sum: both live-in and live-out
 * */
int a[100], sum;
void foo()
{
  int i,sum2,yy,zz;
  sum = 0;
  for (i=0;i<100;i++)
  {
    a[i]=i;
    sum = a[i]+ sum;    
    yy--;
    zz*=a[i];
  }
  sum2=sum+yy+zz;
  a[1]=1;
}
/*
loop carried output dependence 
dep SgExprStatement:sum =((a[i]) + sum); SgExprStatement:sum =((a[i]) + sum); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:sum@7:9->SgVarRefExp:sum@7:9 == 0;||::

non-loop carried  anti dependence, 
dep SgExprStatement:sum =((a[i]) + sum); SgExprStatement:sum =((a[i]) + sum); 1*1 SCALAR_DEP; commonlevel = 1 CarryLevel = 1 SgVarRefExp:sum@7:17->SgVarRefExp:sum@7:9 == 0;||::

Loop carried  anti dependence
dep SgExprStatement:sum =((a[i]) + sum); SgExprStatement:sum =((a[i]) + sum); 1*1 SCALAR_BACK_DEP; commonlevel = 1 CarryLevel = 0 SgVarRefExp:sum@7:9->SgVarRefExp:sum@7:17 <= -1;||::

 */
