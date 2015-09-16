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
#include <string.h>

void initialize(class RectMDArray< double  , 1 , 1 , 1 > &patch)
{
  class Box D0 = patch . getBox();
  int k = 1;
  for (class Point pt(D0 .  getLowCorner ()); D0 .  notDone (pt); D0 .  increment (pt)) 
    patch[pt] = (k++);
}

int main(int argc,char *argv[])
{
  const class Point zero = getZeros();
  const class Point ones = getOnes();
  const class Point negones = ones * -1;
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
  const double C0 = -6.00000;
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
    laplace += ident*((S)^thishft * -1);
  }
  int lb2 = bxdest .  getLowCorner ()[2];
  int k = 0;
  int ub2 = bxdest .  getHighCorner ()[2];
  int arraySize_X = bxdest .  size (0);
  int lb1 = bxdest .  getLowCorner ()[1];
  int j = 0;
  int ub1 = bxdest .  getHighCorner ()[1];
  int arraySize_Y = bxdest .  size (1);
  int lb0 = bxdest .  getLowCorner ()[0];
  int i = 0;
  int ub0 = bxdest .  getHighCorner ()[0];
  int arraySize_Z = bxdest .  size (2);
#pragma 0
  double (*sourceDataPointer)[arraySize_Y][arraySize_X];
  sourceDataPointer = (double (*)[arraySize_Y][arraySize_X])malloc(sizeof(double)*arraySize_X*arraySize_Y*arraySize_Z);    
  memcpy(sourceDataPointer,Asrc.getPointer(),sizeof(double)*arraySize_X*arraySize_Y*arraySize_Z); 
#pragma 0
  double (*destinationDataPointer)[arraySize_Y][arraySize_X];
  destinationDataPointer = (double (*)[arraySize_Y][arraySize_X])malloc(sizeof(double)*arraySize_X*arraySize_Y*arraySize_Z);    
  memcpy(destinationDataPointer,Adest.getPointer(),sizeof(double)*arraySize_X*arraySize_Y*arraySize_Z); 
#pragma 32
  for (k = lb2; k < ub2; ++k) {
    for (j = lb1; j < ub1; ++j) {
      for (i = lb0; i < ub0; ++i) {
        destinationDataPointer[k][j][i] = sourceDataPointer[k + -1][j][i] + sourceDataPointer[k + 1][j][i] + sourceDataPointer[k][j + -1][i] + sourceDataPointer[k][j + 1][i] + sourceDataPointer[k][j][i + -1] + sourceDataPointer[k][j][i + 1] + sourceDataPointer[k][j][i] * -6.00000;
      }
    }
  }
  memcpy(Asrc.getPointer(),sourceDataPointer,sizeof(double)*arraySize_X*arraySize_Y*arraySize_Z); 
  memcpy(Adest.getPointer(),destinationDataPointer,sizeof(double)*arraySize_X*arraySize_Y*arraySize_Z);
  free(sourceDataPointer); 
  free(destinationDataPointer); 
// cout <<" The destination Box" << endl;
// Adest.print();
}
