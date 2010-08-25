#ifndef MatrixTransformMotionFunction_h
#define MatrixTransformMotionFunction_h

#include <Overture.h>

class MatrixTransformMotionFunction
//
// /Purpose: class to provide non-trivial rotation and translation functions
//
{
 public:
  MatrixTransformMotionFunction ();
  MatrixTransformMotionFunction (MatrixTransformMotionFunction& function_);
  //...don't allow implicit conversion from int!
//...ecgs only
//  explicit MatrixTransformMotionFunction (const int& numberOfDimensions_);
  MatrixTransformMotionFunction (const int& numberOfDimensions_);
    
  ~MatrixTransformMotionFunction ();

  virtual real rotationalMotion (const real& time);
  virtual real translationalMotion (const real& time, const int& axis);

  virtual void setTranslationalParameters (const RealArray& trCoeff_,
                                           const RealArray& trFreq_,
					   const RealArray& trOffset_);
  
  virtual void setAngularParameters (const real& angCoeff_,
                                     const real& angFreq_,
				     const real& angOffset_);

  virtual real rotationalVelocity (const real& time);
  virtual real translationalVelocity (const real& time, const int& axis);

 protected:
  
  //...this will prevent operator= from being called
  MatrixTransformMotionFunction& operator= (const MatrixTransformMotionFunction& mtmf);
  
  
  //...parameters:
  // the functions are 
  //
  // x(i)     = trCoeff(i)* sin (2*Pi*trFreq(i)*(t-trOffset(i)), i=1,...,numberOfDimensions
  //
  // theta(i) = angCoeff  * sin (2*Pi*angfreq  *(t-angOffset)
  //
  RealArray trCoeff, trFreq, trOffset;
  real angCoeff, angFreq, angOffset;


  int numberOfDimensions;
  bool coefficientsReDimd, translationalCoefficientsInitialized, angularCoefficientsInitialized;
  
};
#endif


















