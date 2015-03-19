
// This is the second simple test that applies a Laplacian operator
// with 2*DIM+1 size stencil with one ghost cell along each face
// of the data box. Thus the size of the destination box is 
//BLOCKSIZE^DIM and the size of the source box is (BLOCKSIZE+2)^DIM
//FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
//a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile

#define DIM 2
#define BLOCKSIZE 5
#define DOMAINSIZE 7

#include "vector_lite.h"
#include "Shift_lite.H"
#include "Stencil_lite.H" 

int main(int argc, char* argv[])
   {
  // DQ: Modified code to add const.

  // Use of "const" makes the type a SgModifierType (so for now let's keep it simple).
  // const Point zero = getZeros();
     Point zero = getZeros();

     Point lo=zero;
  // Point hi=getOnes()*(DOMAINSIZE-1);
     Point hi=getOnes()*(6);

     Box bxdest(lo,hi); //box low and high corners for destination
  
  // This will grow the box by one ghost
  // along each face and become the box for
  // the source box. 
     Box bxsrc = bxdest.grow(1);

  // source and destination data containers
     RectMDArray<double> Asrc(bxsrc);
     RectMDArray<double> Adest(bxdest);

  // all the coefficients I need for this operation
     const double ident =  1.0;
     const double C0    = -4.0;

  // An expression to recognize: 
  // pair<Shift,double>(zero,C0);

  // This is a simpler interface to interpret (suggested by Anshu).
  // Stencil<double> laplace(pair<Shift,double>(zero,C0));

     Stencil<double> laplace;

     laplace = laplace + (pair<Shift,double>(zero,C0));

     laplace = laplace + (pair<Shift,double>(zero,C0));

#if 0
     Point xdir = getUnitv(0);

     laplace=laplace+(pair<Shift,double>(xdir,ident));
#endif
   }
