#ifndef INTERPOLATE_POINTS_H 
#define INTERPOLATE_POINTS_H

// define some interpolation functions

class OGFunction;
// extern intArray Overture::nullIntegerDistributedArray();
  
int
interpolatePoints(const realArray & positionToInterpolate,
                  const realCompositeGridFunction & u,
		  realArray & uInterpolated, 
                  const Range & R0=nullRange,           
                  const Range & R1=nullRange,
                  const Range & R2=nullRange,
                  const Range & R3=nullRange,
                  const Range & R4=nullRange,
		  intArray & indexGuess=Overture::nullIntegerDistributedArray(),
                  intArray & interpoleeGrid=Overture::nullIntegerDistributedArray(),
                  intArray & wasInterpolated=Overture::nullIntegerDistributedArray());



int
interpolateAllPoints(const realCompositeGridFunction & uFrom,
                     realCompositeGridFunction & uTo );

int
interpolateAllPoints(const realCompositeGridFunction & uFrom,
                     realMappedGridFunction & uTo );

int 
interpolateExposedPoints(CompositeGrid & cg1,  
                         CompositeGrid & cg2, 
                         realCompositeGridFunction & u1,
                         OGFunction *TZFlow=NULL,
                         real t=0.,
                         const bool & returnIndexValues  =FALSE,
                         IntegerArray & numberPerGrid = Overture::nullIntArray() ,
                         intArray & ia = Overture::nullIntegerDistributedArray() );

int
interpolateRefinements( realGridCollectionFunction & u, const Range *C=0,
                        const BoundaryConditionParameters & bcParams = Overture::defaultBoundaryConditionParameters()   );


#endif
