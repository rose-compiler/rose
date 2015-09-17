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


//#define BLOCKSIZE 32
//#define DIM       3
// #define DIM       2

#include "laplacian_lite_v3.h"

void initialize(RectMDArray<double>& patch)
{
  Box D0 = patch.getBox();
  int k=1;
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
      patch[pt] = k++;
}

int main(int argc, char* argv[])
   {
     const Point zero = getZeros();
     const Point ones = getOnes();
     const Point negones = ones*(-1);
     const Point lo = zero;

  // DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
  // Point hi = getOnes()*(BLOCKSIZE-1);
     const int adjustedBlockSize = BLOCKSIZE-1;
     const Point hi = getOnes()* adjustedBlockSize;

     const Box bxdest(lo,hi); //box low and high corners for destination
  
  // This will grow the box by one ghost
  // along each face and become the box for
  // the source box. 
     const Box bxsrc = bxdest.grow(1);

  // source and destination data containers
     RectMDArray<double,1> Asrc(bxsrc);
     RectMDArray<double,1> Adest(bxdest);

  // all the coefficients I need for this operation
     const double ident =  1.0;

  // DQ (2/18/2015): I need the simpler version because the current constant folding does not operate on floating point values.
  // const double C0    = -2.0 * DIM;
     const double C0    = -6.0;

     initialize(Asrc);
     initialize(Adest);

  // cout <<" The source Box" << endl;
  // Asrc.print();
  // cout << endl;

  // build the stencil, and the stencil operator
  // Stencil<double> laplace(wt,shft);
     const array<Shift,DIM> S = getShiftVec();

  // This calls: template <class T> Stencil<T> operator*(T a_coef, Shift a_shift);
     Stencil<double> laplace  = C0 * (S^zero);

     for (int dir=0;dir<DIM;dir++)
        {
          const Point thishft = getUnitv(dir);
       // DQ (2/15/2015): Added operator+=() to support clearer updates of an existing object for compile-time analysis.
       // laplace = laplace + ident*(S^thishft);
       // laplace = laplace + ident*(S^(thishft*(-1)));
          laplace += ident*(S^thishft);
          laplace += ident*(S^(thishft*(-1)));
        }
  // laplace.stencilDump();

  // StencilOperator<double,double, double> op;

  // apply stencil operator
     Stencil<double>::apply(laplace,Asrc,Adest,bxdest);

  // cout <<" The destination Box" << endl;
  // Adest.print();
     return 0;
   }
