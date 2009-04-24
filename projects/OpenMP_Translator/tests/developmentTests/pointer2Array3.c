// array types from a parameter list have to be converted to corresponding pointer types
// to avoid segmentation fault.
// Kernel is extracted from cg of npb2.3 omp c benchmarks.
static int colidx[100][100][50];

static void makea (int colidx[100][100][50])
{
  int i,j,k;
#pragma omp parallel for private(i,j,k)
  for (i = 0; i < 100; i++) 
   for (j = 0; j < 100; j++) 
    for (k = 0; k < 50; k++) 
      colidx[i][j][k] = 0;
}


int main()
{
  makea(colidx);
}

