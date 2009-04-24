// array types from a parameter list have to be converted to corresponding pointer types
// to avoid segmentation fault.
// Kernel is extracted from cg of npb2.3 omp c benchmarks.
static int colidx[100];

static void makea (int colidx[])
{
  int i,j;
#pragma omp parallel for private(i)
  for (i = 1; i <= 100; i++) 
    colidx[i] = 0;
}


int main()
{
  makea(colidx);
}

