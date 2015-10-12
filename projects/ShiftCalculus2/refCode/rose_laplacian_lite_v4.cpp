/*****************************************************************/
/* 09/17/2015 Manually modified by Pei-Hung Lin (lin32@llnl.gov) */
/* Manually fix generated code to have correct array subscript.  */
/* Serve as input code for MPI code generation.                  */
/*****************************************************************/

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

// Decision one: default semantics for sequential code:  run by all processes vs. run only by master process
// #pragma omp mpi_device_default(mpi:all|mpi:master|explicit) 
//
// Three options: default to be run by all, or master, or must explicitly specified!!
// here we use explicit to ensure correctness and avoid misunderstanding.
//   For user productivity, they may choose mpi:all
//
//  For explicit: use 
//      #pragma omp target device (mpi:master) begin .. end
//      #pragma omp target device (mpi:all)    begin .. end
#pragma omp mpi_device_default(explicit) 

void initialize(class RectMDArray< double  , 1 , 1 , 1 > &patch)
{
  class Box D0 = patch . getBox();
  int k = 1;
  for (class Point pt(D0 .  getLowCorner ()); D0 .  notDone (pt); D0 .  increment (pt)) 
    patch[pt] = (k++);
}


int main(int argc,char *argv[])
{

//Transformation: does not change anything at all, just remove the pragmas
#pragma omp target device(mpi:all) begin   // default offload to all processes of MPI, device(mpi:all)
  const class Point zero = getZeros();
  const class Point ones = getOnes();
  const class Point negones = ones * -1;
  const class Point lo(zero);
// DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
// Point hi = getOnes()*(BLOCKSIZE-1);
  const int adjustedBlockSize = 511;
  const class Point hi = getOnes() * adjustedBlockSize;
//box low and high corners for destination
  const class Box bxdest(lo,hi);
// This will grow the box by one ghost
// along each face and become the box for
// the source box. 
  const class Box bxsrc = bxdest .  grow (1);

  class RectMDArray< double  , 1 , 1 , 1 > * Asrc ; 
  class RectMDArray< double  , 1 , 1 , 1 > * Adest; 
#pragma omp target device(mpi:all) end

// source and destination data containers
//#pragma omp target device(mpi) data map(to:Asrc) map(from:Adest)
//
// MPI transformation: split declarations and initialization , guard initialization inside if (rank0)
// How to distinguish between a master within a MPI process vs. a master MPI process?
// A barrier should be added for each master process region!!
#pragma omp device(mpi:master) begin
  Asrc = new RectMDArray< double  , 1 , 1 , 1 > (bxsrc);
  Adest = new RectMDArray< double  , 1 , 1 , 1 > (bxdest);
#pragma omp device(mpi:master) end 

// all the coefficients I need for this operation
  const double ident = 1.0;
// DQ (2/18/2015): I need the simpler version because the current constant folding does not operate on floating point values.
// const double C0    = -2.0 * DIM;
  const double C0 = -6.00000;

#pragma omp target device(mpi:master) begin
  initialize(*Asrc);
  initialize(*Adest);
#pragma omp target device(mpi:master) end 

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
// laplace.stencilDump();
// StencilOperator<double,double, double> op;

// MPI transformation: split declarations and initialization , guard initialization inside if (rank0)
// all variables can be visible to later code. otherwise scope is limited to if body!!
#pragma omp device(mpi:master) begin
  int lb2src = bxsrc .  getLowCorner ()[2];
  int lb1src = bxsrc .  getLowCorner ()[1];
  int lb0src = bxsrc .  getLowCorner ()[0];

  int arraySize_X_src = bxsrc .  size (0);
  int arraySize_Y_src = bxsrc .  size (1);
  int arraySize_Z_src = bxsrc .  size (2);

 //destination 
  int lb0 = bxdest .  getLowCorner ()[0];
  int lb1 = bxdest .  getLowCorner ()[1];
  int lb2 = bxdest .  getLowCorner ()[2];

  int ub0 = bxdest .  getHighCorner ()[0];
  int ub1 = bxdest .  getHighCorner ()[1];
  int ub2 = bxdest .  getHighCorner ()[2];

  int arraySize_X = bxdest .  size (0);
  int arraySize_Y = bxdest .  size (1);
  int arraySize_Z = bxdest .  size (2);

  int i = 0;
  int k = 0;
  int j = 0;

  double *sourceDataPointer = Asrc -> getPointer();
  double *destinationDataPointer = Adest -> getPointer();

#pragma omp device(mpi:master) end

// TODO: more fine design for nested parallelism

#pragma omp target device(mpi:all) map(to:lb0src, lb1src, lb2src, lb2, ub2,lb1,ub1,lb0,ub0, arraySize_X, arraySize_Y, arraySize_X_src, arraySize_Y_src)\
map(to:sourceDataPointer[lb0src:arraySize_X_src][lb1src:arraySize_Y_src][lb2src:arraySize_Z_src] dist_data(DUPLICATE, DUPLICATE, BLOCK|1)) \
map(from:destinationDataPointer[lb0:arraySize_X][lb1:arraySize_Y][lb2:arraySize_Z] dist_data(DUPLICATE, DUPLICATE, BLOCK))
#pragma omp parallel for 
  for (k = lb2; k <= ub2; ++k) { // loop to be distributed must match the dimension being distributed (3rd dimension).
    for (j = lb1; j <= ub1; ++j) {
      for (i = lb0; i <= ub0; ++i) {
        destinationDataPointer[arraySize_X * (arraySize_Y * k + j) + i] = 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] + 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] + 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] + 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] + 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] + 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] + 
            sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] * -6.00000;
      }
    }
  }
// cout <<" The destination Box" << endl;
// Adest.print();
}
