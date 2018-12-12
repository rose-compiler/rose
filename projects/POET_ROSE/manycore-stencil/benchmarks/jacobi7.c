/*
	StencilProbe Heat Equation
	Implements 7pt stencil from Chombo's heattut example.
*/
void jacobi7(const int nx,const int ny, int nz, float* A0,const int timesteps,const float* B,const int ldb, float* Anext,const int ldc) 
{
  float *temp_ptr;
  int i, j, k, t;	
  float *l0, *lnext;
  float fac = 6.0/(A0[0]*A0[0]);

  #pragma stencil-iterative(t,0,timesteps) [float fac] (float, (A0 l0), (Anext lnext)) D(k,0,nz,1) D(j,0,ny,1) S(i,0,nx,1) 
  for (t = 0; t < timesteps; t++) {
    if (t%2 == 0) { l0 = A0; lnext = Anext; }
    else {lnext = A0; l0 = Anext; }
    for (k = 1; k < nz - 1; k++) {
      for (j = 1; j < ny - 1; j++) {
	for (i = 1; i < nx - 1; i++) {
	  lnext[i+nx*(j+ ny*k)] = 
	    l0[i+nx*(j+ ny*( k + 1))] +
	    l0[i+nx*(j+ ny*( k - 1))] +
	    l0[i+nx*((j + 1)+ ny*k)] +
	    l0[i+nx*((j - 1)+ ny*k)] +
	    l0[i+1+nx*(j+ ny*k)] +
	    l0[(i - 1)+nx*(j+ ny*k)]
	    -  l0[i+nx*(j+ ny*k)] *fac ;
	}
      }
    }
  }
}

