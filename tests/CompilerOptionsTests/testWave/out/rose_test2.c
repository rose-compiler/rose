#pragma wave trace ( enable )
int a[100UL][100UL];

void foo()
{
  int i;
  int j;
  int hypre__nx;
  int hypre__ny;
#define	HYPRE_BOX_SMP_PRIVATE	i,j
#define	HYPRE_SMP_PRIVATE	HYPRE_BOX_SMP_PRIVATE,hypre__nx,hypre__ny
  
#pragma omp parallel for private ( HYPRE_SMP_PRIVATE )
  for (i = 0; i < 100; i++) 
    for (j = 0; j < 100; j++) {
      hypre__nx = i;
      hypre__ny = j;
      (a[i])[j] = (hypre__nx + hypre__ny);
    }
}

