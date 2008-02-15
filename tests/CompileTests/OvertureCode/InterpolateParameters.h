#ifndef InterpolateParameters_h
#define InterpolateParameters_h

#include <Overture.h>
#include <NameList.h>
//#include <AMR++.h>

class InterpolateParameters
{
 public:

  InterpolateParameters (const int numberFOfDimensions_=2, const bool debug_ = LogicalFalse);
  ~InterpolateParameters();

  enum InterpolateType
  {
    defaultValue,
    polynomial,
    fullWeighting,
    nearestNeighbor,
    injection,
    numberOfInterpolateTypes
  };

  enum InterpolateOffsetDirection
  {
    offsetInterpolateToLeft,
    offsetInterpolateToRight,
    numberOfInterpolateOffsetDirections
  };
    
  int interactivelySetParameters ();

  void display () const;

  int get( const GenericDataBase & dir, const aString & name);

  int put( GenericDataBase & dir, const aString & name) const;

  static const int defaultAmrRefinementRatio;
  static const GridFunctionParameters::GridFunctionType defaultGridCentering;
  static const InterpolateParameters::InterpolateType defaultInterpolateType;
  static const int defaultInterpolateOrder;
  static const int defaultMaxRefinementRatio;
  static const int defaultNumberOfDimensions;
  static const real coeffEps;
  static const InterpolateOffsetDirection defaultInterpolateOffsetDirection;

  bool debug;

 protected:
  //...class data is protected; use access functions to get or set values

  IntegerArray amrRefinementRatio__;  
  int maxRefinementRatio__;
  int numberOfDimensions__;
  GridFunctionParameters::GridFunctionType gridCentering__;
  InterpolateType interpolateType__;
  int interpolateOrder__;

//bool preComputeAllCoefficients__;
  bool useGeneralInterpolationFormula__;

 public:  
  //...class member access functions

  void setAmrRefinementRatio (const IntegerArray& amrRefinementRatio_);
  void setNumberOfDimensions (const int numberOfDimensions_=defaultNumberOfDimensions);
  
  void setInterpolateType    (const InterpolateParameters::InterpolateType interpolateType_ =
			      InterpolateParameters::defaultInterpolateType);
  void setInterpolateOrder   (const int interpolateOrder_ = 
			      InterpolateParameters::defaultInterpolateOrder);
  void setGridCentering      (const GridFunctionParameters::GridFunctionType gridCentering_ = 
			      InterpolateParameters::defaultGridCentering);
//void setPreComputeAllCoefficients (const bool TrueOrFalse = LogicalFalse);
  void setUseGeneralInterpolationFormula (const bool TrueOrFalse = LogicalFalse);
  
  
  int amrRefinementRatio (const int axis) const                   { return amrRefinementRatio__(axis);};
  GridFunctionParameters::GridFunctionType gridCentering () const { return gridCentering__;};
  InterpolateType interpolateType () const                        { return interpolateType__;};
  int interpolateOrder () const                                   { return interpolateOrder__;};
  int numberOfDimensions () const                                 { return numberOfDimensions__;};
  bool useGeneralInterpolationFormula () const                    { return useGeneralInterpolationFormula__;};
  

  NameList nl;
};

#endif
  

  









