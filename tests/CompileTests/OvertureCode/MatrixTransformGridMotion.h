#ifndef MatrixTransformGridMotion_h
#define MatrixTransformGridMotion_h

#include <Overture.h>
#include "MatrixTransform.h"
#include "GenericGridMotion.h"
#include "MatrixTransformGridMotionParameters.h"

class MatrixTransformGridMotion : public GenericGridMotion
//
//\begin{>MatrixTransformGridMotion.tex}{\subsection{\MatrixTransformGridMotion class}}
//\no function header:
//   This is a derived GridMotion class that describes the motion of component grids
//   by using the MatrixTransform Mapping class.
//
// /Author: D L Brown 980323
//
//\end{MatrixTransformGridMotion.tex}
{
 public:
  MatrixTransformGridMotion ();
  MatrixTransformGridMotion (CompositeGrid & cg_, 
                             const int& numberOfLevels_,
			     MatrixTransformGridMotionParameters* params_);

  MatrixTransformGridMotion (const MatrixTransformGridMotion& ggm);
  MatrixTransformGridMotion& operator=(const MatrixTransformGridMotion& ggm);

  virtual ~MatrixTransformGridMotion ();

  void initialize (CompositeGrid & cg_,
                   const int& numberOfLevels_,
		   MatrixTransformGridMotionParameters* params_);

  virtual void moveMappings (const real & time,
                             const real & timestep, 
			     const int & level);


  virtual void moveMappings (const real & time,
                             const real & timestep,
			     const realCompositeGridFunction & u,
			     const int & level);


  virtual void getAnalyticVelocity(realMappedGridFunction& velocity,
                                   const int& grid,
				   const int& level,
				   const real& time,
				   CompositeGrid** cgMoving,
				   const Index& I1,
				   const Index& I2,
				   const Index& I3);
  

  //...this function is used to initialize moving Mappings and their parameters

  virtual void initializeMappings ();

 protected:


  MatrixTransformGridMotionParameters* params;
  bool paramsSet;
  //...units of rotationRate are rotations/(unit time)
  real rotationRate, translationRate[3];
  
};


#endif
