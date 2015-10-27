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
// #define DIM       2
#include "laplacian_lite_v3.h"

int main(int argc,char *argv[])
{
  const class Point zero = getZeros();
  const class Point ones = getOnes();
  const class Point negones = ones * - 1;
  const class Point lo(zero);
// DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
// Point hi = getOnes()*(BLOCKSIZE-1);
  const int adjustedBlockSize = 31;
  const class Point hi = getOnes() * adjustedBlockSize;
//box low and high corners for destination
  const class Box bxdest(lo,hi);
// This will grow the box by one ghost
// along each face and become the box for
// the source box. 
  const class Box bxsrc = bxdest .  grow (1);
// source and destination data containers
  class RectMDArray< double  , 1 , 1 , 1 > Asrc(bxsrc);
  class RectMDArray< double  , 1 , 1 , 1 > Adest(bxdest);
// all the coefficients I need for this operation
  const double ident = 1.0;
// DQ (2/18/2015): I need the simpler version because the current constant folding does not operate on floating point values.
// const double C0    = -2.0 * DIM;
  const double C0 = - 6.00000;
  initialize(Asrc);
  initialize(Adest);
// cout <<" The source Box" << endl;
// Asrc.print();
// cout << endl;
// build the stencil, and the stencil operator
// Stencil<double> laplace(wt,shft);
  const std::array< Shift  , 3 > S = getShiftVec();
// This calls: template <class T> Stencil<T> operator*(T a_coef, Shift a_shift);
  class Stencil< double  > laplace = C0*((S)^(zero));
  for (int dir = 0; dir < 3; dir++) {
    const class Point thishft = getUnitv(dir);
// DQ (2/15/2015): Added operator+=() to support clearer updates of an existing object for compile-time analysis.
// laplace = laplace + ident*(S^thishft);
// laplace = laplace + ident*(S^(thishft*(-1)));
    laplace += ident*((S)^(thishft));
    laplace += ident*((S)^thishft * - 1);
  }
  double *sourceDataPointer = Asrc . getPointer();
  double *destinationDataPointer = Adest . getPointer();
  int arraySize_X = 32;
  int arraySize_Y = 32;
  int arraySize_Z = 32;
  int lbi = bxdest .  getLowCorner ()[0];
  int ubi = bxdest .  getHighCorner ()[0];
  int lbj = bxdest .  getLowCorner ()[1];
  int ubj = bxdest .  getHighCorner ()[1];
  int lbk = bxdest .  getLowCorner ()[2];
  int ubk = bxdest .  getHighCorner ()[2];
  int i;
  int j;
  int k;
#pragma omp target device (gpu0) map(in:arraySize_X,arraySize_Y,arraySize_Z,sourceDataPointer[0:arraySize_X*arraySize_Y*arraySize_Z]) map(inout:destinationDataPointer[0:arraySize_X*arraySize_Y*arraySize_Z])

#pragma omp parallel for collapse(3) private(i,j,k)
  for (k = lbk; k < ubk; ++k) {
    for (j = lbj; j < ubj; ++j) {
      for (i = lbi; i < ubi; ++i) {
        destinationDataPointer[k * arraySize_Y + (j * arraySize_X + i)] = sourceDataPointer[(k + - 1) * arraySize_Y + (j * arraySize_X + i)] + sourceDataPointer[(k + 1) * arraySize_Y + (j * arraySize_X + i)] + sourceDataPointer[(j + - 1) * arraySize_X + i] + sourceDataPointer[(j + 1) * arraySize_X + i] + sourceDataPointer[k * arraySize_Y + (j * arraySize_X + (i + - 1))] + sourceDataPointer[k * arraySize_Y + (j * arraySize_X + (i + 1))] + sourceDataPointer[k * arraySize_Y + (j * arraySize_X + i)] * - 6.00000;
      }
    }
  }
// cout <<" The destination Box" << endl;
// Adest.print();
}
