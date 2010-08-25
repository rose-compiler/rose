#ifndef Interpolate_h
#define Interpolate_h

#include <Overture.h>
#include <NameList.h>
//#include <AMR++.h>

#include "InterpolateParameters.h"
#include "axisDefs.h"
#include "Display.h"

//extern InterpolateParameters defaultInterpolateParameters;

class Interpolate
{
 public:

  bool debug;
  bool timing;

  Interpolate();

  Interpolate (const InterpolateParameters& interpParams_, 
               const bool timing=LogicalFalse);
  
  ~Interpolate();

  int initialize (const InterpolateParameters& interpParams_,
		  const bool timing=LogicalFalse);
		  

  int interpolateCoarseToFine (realArray&                   fineGridArray,
                               const Index                  Iv[3],
			       const realArray&             coarseGridArray,
			       const IntegerArray&          amrRefinementRatio_=Overture::nullIntArray());

    
  int interpolateFineToCoarse (realArray&                   coarseGridArray,
                               const Index                  Iv[3],
			       const realArray&             fineGridArray,
			       const IntegerArray&          amrRefinementRatio_=Overture::nullIntArray());
  
 protected:


  RealArray coeff;  //interpolation coefficients
  int interpolateOrder;
  int numberOfDimensions;
  bool preComputeAllCoefficients;
  bool useGeneralInterpolationFormula;

  IntegerArray amrRefinementRatio;
  GridFunctionParameters::GridFunctionType gridCentering;
  InterpolateParameters::InterpolateType interpolateType;

  int initializeCoefficients (const int maxRefinementRatio_, 
			      const int interpolateOrder_, 
			      const int numberOfDimensions_,
			      const int *interpolateOffset_);

  int computeIndexes (const realArray& uc,
		      int* lm, Index* Jf, Index* Jc, const Index* If,
		      const int* lmr, const int R, const int* A,
		      const int* r, const int* stride, const int* extra, const int* offset,
		      InterpolateParameters::InterpolateOffsetDirection* iod);

  int displayEverything (const int* r, const int* extra, const int* offset, const int* lm, const int* lmr,
                         const Index* Iv, const Index* If, const Index* Jf, const Index* Jc);
  
};

  
#endif
