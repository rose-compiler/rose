int i,j;
int a[100][100];

void foo()
{
  for (i=0;i<100;i++)
    for (j=0;j<100;j++)
      a[i][j]=a[i][j]+1;
}
/*
 * final dependence graph:
dep SgExprStatement:(a[i])[j] =(((a[i])[j]) + 1); 
    SgExprStatement:(a[i])[j] =(((a[i])[j]) + 1); 
2*2 ANTI_DEP; commonlevel = 2  +precise  CarryLevel = 2 //level =0,1,2 no loop carried dependence!!
  SgPntrArrRefExp:((a[i])[j])
  SgPntrArrRefExp:(a[i])[j]== 0;* 0;:* 0;== 0;:::

Two accesses: distance =0, non loop-carried
  write after read for a[i]: anti-dependence

 */

