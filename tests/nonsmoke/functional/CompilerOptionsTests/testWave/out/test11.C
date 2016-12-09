int a[100][100];
void foo()
{
  int i,j;
  int hypre__nx,hypre__ny,hypre__nz;
#define HYPRE_BOX_SMP_PRIVATE i,j
#include "test11.h"
  for (i=0;i<100; i++)
    for (j=0;j<100; j++)
    {
      hypre__nx =i;
      hypre__ny=j;
      a[i][j]=hypre__nx+hypre__ny;
    }
}

