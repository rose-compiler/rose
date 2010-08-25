#ifndef FOURIER_OPERATORS_H
#define FOURIER_OPERATORS_H

#include "Overture.h"


// ===================================================================================
//    Apply Fourier operations:
//      o forward and reverse transforms
//      o derivatives and integrals in fourier space 
//   
//   arrays are assumed to be of the form
//      u(nx,ny,nz,C0)  : C0=optional components
//
// ===================================================================================
class FourierOperators
{
 public:
  FourierOperators(const int & numberOfDimensions, const int & nx, const int & ny=1, const int & nz=1);
  ~FourierOperators();

  void setDimensions(const int & numberOfDimensions, const int & nx, const int & ny=1, const int & nz=1);

  void setPeriod(const real & xPeriod, 
                 const real & yPeriod=twoPi, 
                 const real & zPeriod=twoPi ); // set period, default is 2*pi

  // Change the Ranges over which the transforms are performed. This will also change
  // the number of points.
  void setDefaultRanges(const Range & R1,                  // use this instead of default Range(0,nx-1)
			const Range & R2=nullRange,        // use this instead of default Range(0,ny-1)
			const Range & R3=nullRange );

  // real space to fourier space (forward transform)
  void realToFourier( const RealDistributedArray & u, RealDistributedArray & uHat, const Range & Components=nullRange );

  // fourier space to real space (backward transform)
  void fourierToReal(const RealDistributedArray & uHat, 
                     RealDistributedArray & u, 
                     const Range & Components=nullRange );

  // compute a derivative (or integral) in Fourier space
  void fourierDerivative(const RealDistributedArray & uHat, 
                         RealDistributedArray & uHatDerivative, 
			 const int & xDerivative=1, 
			 const int & yDerivative=0, 
			 const int & zDerivative=0, 
			 const Range & Components=nullRange );
  
  // compute 
  //       (Laplacian)^power uHat
  // where power can be positive or negative
  void fourierLaplacian(const RealDistributedArray & uHat, 
			RealDistributedArray & uHatLaplacian, 
			const int & power=1,
			const Range & Components=nullRange );
  
  void transform(const int & forwardOrBackward,
                 const RealDistributedArray & u, 
		 RealDistributedArray & uHat, 
		 const Range & Components );
  
 private:
  int numberOfDimensions,nx,ny,nz;
  real xPeriod, yPeriod, zPeriod;
  Range R1,R2,R3;
  bool initialized;
  void initialize();
  RealDistributedArray uTemp;
  RealDistributedArray wsavex,wsavey,wsavez;
  RealDistributedArray kSquared,kxDerivative,kyDerivative,kzDerivative;
};

#endif
