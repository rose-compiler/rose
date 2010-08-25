#ifndef MatrixTransformGridMotionParameters_h
#define MatrixTransformGridMotionParameters_h

#include "GenericGridMotionParameters.h"
#include "MatrixTransformMotionFunction.h"

class MatrixTransformGridMotionParameters : public GenericGridMotionParameters
{
//
//\begin{>MatrixTransformGridMotionParameters.tex}{\subsection{\MatrixTransformGridMotionParameters class}}
//\no function header:
//   This is a container class to store parameters for the \MatrixTransformGridMotion
//   class. Some of the parameters are directly accessable to the user:
//
// /rotationRate(grid):  a RealArray containing the rotation rate for each grid (assumed 2D at the moment)
// /translationRate(grid,axis): a RealArray containing the components of the translation rate for 
//       each grid
//\end{MatrixTransformGridMotionParameters.tex}

 public:
  MatrixTransformGridMotionParameters ();
  MatrixTransformGridMotionParameters (const MatrixTransformGridMotionParameters & params_);
  MatrixTransformGridMotionParameters& 
    operator= (const MatrixTransformGridMotionParameters & params_);
//...ecgs only
//  explicit MatrixTransformGridMotionParameters (const CompositeGrid& compositeGrid_);
// ***obsolete
  MatrixTransformGridMotionParameters (const CompositeGrid& compositeGrid_);
  MatrixTransformGridMotionParameters (const int& numberOfDimensions,
                                       const int& numberOfGrids);
  
  virtual ~MatrixTransformGridMotionParameters ();
  
  RealArray rotationRate;
  RealArray translationRate;

  void setMotionFunction (const int& grid,
			  MatrixTransformMotionFunction* motionFunction__);

  inline bool useMotionFunction (const int& grid);
  
  MatrixTransformMotionFunction* motionFunction(const int& grid){return motionFunction_[grid];};

  bool setupWizard ();
  
 protected:

  MatrixTransformMotionFunction** motionFunction_;
  LogicalArray useMotionFunction_;
  bool motionFunctionInitialized;
  
  void setupMotionFunction();
  MatrixTransformMotionFunction** copyMotionFunction (MatrixTransformMotionFunction** mf);
  void destructMotionFunction();
  

};


//\begin{>>MatrixTransformGridMotionParameters.tex}{\subsubsection{useMotionFunction}}
inline bool MatrixTransformGridMotionParameters::
useMotionFunction (const int& grid)
//
// /Purpose: access function for useMotionFunction--tells whether a 
//   motionFunction is used to describe the motion on this grid
//  /grid: which grid
//\end{MatrixTransformGridMotionParameters.tex}
{
  return useMotionFunction_(grid);
} ;


#endif





