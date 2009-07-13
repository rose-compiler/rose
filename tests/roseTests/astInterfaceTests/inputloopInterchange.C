void OUT__1__6119__(int ri, double *rp, int stencil_size, int hypre__m, const double *Ap_0)
{
  int si , ii , jj , kk ;
  // the following 4-level loop nest is to be interchanged
  for (si = 0; si < stencil_size; si++)
    for (kk = 0; kk < hypre__m; kk++)
      for (jj = 0; jj < hypre__m; jj++)
        for (ii = 0; ii < hypre__m; ii++)
          rp[(ri + ii) + jj  + kk ] -= Ap_0[ii + jj  + kk];
} 
