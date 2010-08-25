#ifndef TRIDIAGONAL_SOLVER_H
#define TRIDIAGONAL_SOLVER_H

#include "Overture.h"

class TridiagonalSolver
{
 public:
  enum SystemType
  {
    normal=0,
    extended,
    periodic
  };
  
  TridiagonalSolver();
  ~TridiagonalSolver();
  
  int factor( realArray & a, 
	      realArray & b, 
	      realArray & c, 
	      const SystemType & type=normal, 
	      const int & axis=0,
              const int & block=1 );
  int solve( const realArray & r,        // this is not const
             const Range & R1=nullRange, 
             const Range & R2=nullRange, 
             const Range & R3=nullRange );

  virtual real sizeOf( FILE *file=NULL ) const ; // return number of bytes allocated, print info to a file

 protected:

  int tridiagonalFactor();
  int tridiagonalSolve( realArray & r );
  int periodicTridiagonalFactor();
  int periodicTridiagonalSolve( realArray & r );
  
  int invert(realArray & d, const int & i1 );
  int invert(realArray & d, const Index & I1, const Index & I2, const Index & I3 );

  realArray multiply( const realArray & d, const int & i1, const realArray & e, const int & j1);
  realArray matrixVectorMultiply( const realArray & d, const int & i1, const realArray & e, const int & j1);
  realArray matrixVectorMultiply( const realArray & d, const int & i1, const realArray & e);

  realArray multiply(const realArray & d, const Index & I1, const Index & I2, const Index & I3, 
                     const realArray & e, const Index & J1, const Index & J2, const Index & J3);
  realArray matrixVectorMultiply(const realArray & d, const Index & I1, const Index & I2, const Index & I3, 
                                 realArray & e, const Index & J1, const Index & J2, const Index & J3);

  int blockFactor();
  int blockSolve(realArray & r);
  int blockPeriodicFactor();
  int blockPeriodicSolve(realArray & r);
  int scalarBlockFactor(int i1, int i2, int i3 );
  int scalarBlockSolve(realArray & r, int i1, int i2, int i3);
  int scalarBlockPeriodicFactor(int i1, int i2, int i3);
  int scalarBlockPeriodicSolve(realArray & r, int i1, int i2, int i3);
  
  SystemType systemType;
  int axis;
  realArray a,b,c,w1,w2;
  Range Iv[3], &I1, &I2, &I3;

  int blockSize;  // we can do 1x1, 2x2 and 3x3 blocks
  bool scalarSystem;

  bool useOptimizedC;
  
};

#endif
