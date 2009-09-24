  for (i=1;i<n;i++)
    for (j=1;j<m;j++)
      a[i][j]=a[i][j-1]+a[i-1][j];
/*
dep SgExprStatement @3--> SgExprStatement @3
    2*2 TRUE_DEP; commonlevel = 2 CarryLevel = 1  
    SgPntrArrRefExp:a[i])[j] @3:14->SgPntrArrRefExp:a[i][j - 1] @3:19
     == 0; *   0;
     *  0; == -1;

dep SgExprStatement @3--> SgExprStatement @3
    2*2 TRUE_DEP; commonlevel = 2 CarryLevel = 0  
    SgPntrArrRefExp:a[i][j]@3:14->SgPntrArrRefExp:a[i - 1][j]@3:31
    == -1;  * 0;
    *  0 ; == 0;
*/
