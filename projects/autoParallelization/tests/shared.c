/*
 * dependence graph:
 */
void foo()
{
  int i,x;
  int a[100];
  for (i=0;i<100;i++) 
  { 
    a[i]=a[i]+1; 
  }  
}  
/*
  non loop carried anti dependence for array accesses  : level =1 > 0 
dep SgExprStatement:a[i] =((a[i]) + 1); SgExprStatement:a[i] =((a[i]) + 1); 1*1 ANTI_DEP; commonlevel = 1 CarryLevel = 1  Is precise SgPntrArrRefExp:(a[i])@10:11->SgPntrArrRefExp:a[i]@10:9 == 0;||::
 */
