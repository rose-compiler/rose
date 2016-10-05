
#if 0
static void compute_initial_conditions(dcomplex u0[NZ][NY][NX], int d[3]) {

/*--------------------------------------------------------------------
c-------------------------------------------------------------------*/

/*--------------------------------------------------------------------
c Fill in array u0 with initial conditions from 
c random number generator 
c-------------------------------------------------------------------*/

    int k;
    double x0, start, an, dummy;
    static double tmp[NX*2*MAXDIM+1];
    int i,j,t;
      
    start = SEED;
/*--------------------------------------------------------------------
c Jump to the starting element for our first plane.
c-------------------------------------------------------------------*/
    ipow46(A, (zstart[0]-1)*2*NX*NY + (ystart[0]-1)*2*NX, &an);
    dummy = randlc(&start, an);
    ipow46(A, 2*NX*NY, &an);
      
/*--------------------------------------------------------------------
c Go through by z planes filling in one square at a time.
c-------------------------------------------------------------------*/
    for (k = 0; k < dims[0][2]; k++) {
	x0 = start;
        vranlc(2*NX*dims[0][1], &x0, A, tmp);
	
	t = 1;
	for (j = 0; j < dims[0][1]; j++)
	  for (i = 0; i < NX; i++) {

    // Error ROSE unparses this as:
    // ((u0[k])[j])[i].::__rose_generated_structure_tag_name_22::real = (tmp[t++]);
    // ((u0[k])[j])[i].::__rose_generated_structure_tag_name_22::imag = (tmp[t++]);
	    u0[k][j][i].real = tmp[t++];
	    u0[k][j][i].imag = tmp[t++];
	  }
	      
        if (k != dims[0][2]) dummy = randlc(&start, an);
    }
}
#endif

class complex 
   {
     public:
          float real;
          float imaginary;
   };

void foo()
   {
/*
Unparses to be:
  class ::complex X[10];
  X[0].::complex::real = (0.0);
  class __rose_generated_structure_tag_name_0 {
  public: float real;
  float imaginary;}complexArray[10];
  complexArray[0].__rose_generated_structure_tag_name_0::real = (0);
*/
     complex X[10];
     X[0].real = 0.0;

     class 
        {
          public:
               float real;
               float imaginary;
        } complexArray[10];

     complexArray[0]. real = 0;
   }
