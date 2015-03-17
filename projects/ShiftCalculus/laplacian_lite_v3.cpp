// This is the simple version of laplacian.cpp that builds 
// the smallest reasonable AST.  This simpler AST make it 
// easier to work with in the context of understanding 
// the use of the DSL and the interpretation of it's semantics 
// to implement the code generation for the DSL.

// This is the second simple test that applies a Laplacian operator
// with 2*DIM+1 size stencil with one ghost cell along each face
// of the data box. Thus the size of the destination box is 
//BLOCKSIZE^DIM and the size of the source box is (BLOCKSIZE+2)^DIM
//FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
//a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile


#define BLOCKSIZE 32
#define DIM       3

#include "laplacian_lite_v3.h"

int main(int argc, char* argv[])
   {
     Point zero = getZeros();
     Point ones = getOnes();
     Point negones = ones*(-1);
     Point lo = zero;

  // DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
  // Point hi = getOnes()*(BLOCKSIZE-1);
     int adjustedBlockSize = BLOCKSIZE-1;
     Point hi = getOnes()* adjustedBlockSize;

     Box bxdest(lo,hi); //box low and high corners for destination
  
  // This will grow the box by one ghost
  // along each face and become the box for
  // the source box. 
     Box bxsrc=bxdest.grow(1);
  
  // source and destination data containers
     RectMDArray<double,1> Asrc(bxsrc);
     RectMDArray<double,1> Adest(bxdest);

  // all the coefficients I need for this operation
     double ident=1.0;
     double C0=-4.0;

     initialize(Asrc);
     initialize(Adest);

  // cout <<" The source Box" << endl;
  // Asrc.print();
  // cout << endl;

  // build the stencil, and the stencil operator
  // Stencil<double> laplace(wt,shft);
     array<Shift,DIM> S = getShiftVec();
     Stencil<double> laplace = C0*(S^zero);

     for (int dir=0;dir<DIM;dir++)
        {
          Point thishft = getUnitv(dir);
          laplace = laplace + ident*(S^thishft);
          laplace = laplace + ident*(S^(thishft*(-1)));
        }
     laplace.stencilDump();
  
  // StencilOperator<double,double, double> op;

  // apply stencil operator
     Stencil<double>::apply(laplace,Asrc,Adest,bxdest);

  // cout <<" The destination Box" << endl;
  // Adest.print();
   }
