// Contributed by Allan Porterfield
// 1/26/2010
void sparselu_par_call(float **BENCH)
{
  int ii, jj, kk;

#pragma omp parallel
  {
#pragma omp single nowait
#pragma omp task untied
    {
      for (kk=0; kk<100; kk++)
        {
          lu0(BENCH[kk]);
          for (jj=kk+1; jj<100; jj++)
            {
#pragma omp task untied firstprivate(kk, jj) shared(BENCH)
              if (BENCH[jj] != 0)
                {
                  fwd(BENCH[kk], BENCH[jj]);
                }
            }
        }
#pragma omp taskwait
    }
  }
}

