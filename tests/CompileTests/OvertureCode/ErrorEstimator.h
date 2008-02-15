#ifndef ERROR_ESTIMATOR_H
#define ERROR_ESTIMATOR_H

#include "Overture.h"
#include "PlotStuffParameters.h"

class PlotStuff;
// class PlotStuffParameters;
class GenericGraphicsInterface;
class InterpolateRefinements;

class ErrorEstimator
{
 public:

  enum
  {
    defaultNumberOfSmooths=-99999
  };
  
  enum ErrorFunctionEnum
  {
    twoSolidCircles,
    diagonal,
    cross,
    plus,
    hollowCircle
  };
  enum FunctionEnum
  {
    topHat
  };

  ErrorEstimator(InterpolateRefinements & interpolateRefinements);
  ~ErrorEstimator();


  // build an error with a predefined shape (for testing):
  int computeErrorFunction( realGridCollectionFunction & error, ErrorFunctionEnum type );
  int computeFunction( realGridCollectionFunction & u, FunctionEnum type, real t=0. );
  
  // base error on undivided differences:
  int computeErrorFunction( realCompositeGridFunction & u, realCompositeGridFunction & error,
			    int numberOfSmooths =1 );
  int computeErrorFunction( realGridCollectionFunction & u, realGridCollectionFunction & error,
			    int numberOfSmooths =defaultNumberOfSmooths  );
  
  int displayParameters(FILE *file = stdout ) const;

  int plotErrorPoints(  realGridCollectionFunction & error, real errorThreshhold,
			PlotStuff & ps, PlotStuffParameters & psp );
  

  int setDefaultNumberOfSmooths( int numberOfSmooths );
  
  // set scale factors for each component. Use these when scaling the errors.
  int setScaleFactor( RealArray & scaleFactor );
  
  int setTopHatParameters( real topHatCentre[3], real topHatVelocity[3], real topHatRadius, 
                           real topHatRadiusX_=0. , real topHatRadiusY_=0., real topHatRadiusZ_=0.);

  int setWeights( real weightFirstDifference, real weightSecondDifference );
  
  int get( const GenericDataBase & dir, const aString & name);

  int put( GenericDataBase & dir, const aString & name) const;

  int update( GenericGraphicsInterface & gi );

  int debug;
  
 protected:

  int interpolateAndApplyBoundaryConditions( realCompositeGridFunction & error, CompositeGridOperators & op );
  int smooth( realGridCollectionFunction & error );

  real weightFirstDifference, weightSecondDifference;
  
  real topHatCentre[3], topHatVelocity[3];
  real topHatRadius, topHatRadiusX, topHatRadiusY, topHatRadiusZ;

  RealArray scaleFactor;
  
  InterpolateRefinements *interpolateRefinements;

  int defaultNumberOfSmoothingSteps;

};


#endif
