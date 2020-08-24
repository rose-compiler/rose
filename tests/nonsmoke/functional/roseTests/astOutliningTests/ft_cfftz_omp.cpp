#include <assert.h>
  
typedef struct { double real; double imag; } dcomplex;

//Extracted from NPB, FT
/* COMMON block: ucomm */

//static dcomplex u[64];
dcomplex u[64];

static void cfftz();
static void cfftz()
{
  int mx = (int)(u[0].real);

  assert (mx==6);
}

static int ilog2(int n)
{
  int nn;
  int lg;
  if (n == 1) {
    return 0;
  }
  lg = 1;
  nn = 2;
  while(nn < n){
    nn = nn << 1;
    lg++;
  }
  return lg;
}

static void fft_init(int n)
{
   int m = ilog2(n);
   u[0].real = (double)m;
}

int main()
{
  fft_init(64);
#pragma omp for
for (int i=0; i<1; i++)
  {
    cfftz();
  }
  return 0; 
}
