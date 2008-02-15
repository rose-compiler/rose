#ifndef SPLINE_MAPPING_H
#define SPLINE_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a Spline Curve in 1D, 2D or 3D
//-------------------------------------------------------------
class SplineMapping : public Mapping
{

public:

  enum ParameterizationType
  {
    index,
    arcLength,
    userDefined
  };
  
  enum EndCondition
  {
    monotoneParabolicFit=0,
    firstDerivative,
    secondDerivative,
    free              // for old spline routines only.    
  };

  SplineMapping(const int & rangeDimension=2 );

  // Copy constructor is deep by default
  SplineMapping( const SplineMapping &, const CopyType copyType=DEEP );

  ~SplineMapping();

  SplineMapping & operator =( const SplineMapping & X0 );

  int getNumberOfKnots() const;
  
  // set 'arclength' parameterization parameters
  int parameterize( const real & arcLengthWeight=1., const real & curvatureWeight=0. );
  
  // rotate about a given axis
  int rotate( const int & axis, const real & theta );
  
  // scale 
  int scale(const real & scalex=1., 
	    const real & scaley=1., 
	    const real & scalez=1. );

  // shift in space
  int shift(const real & shiftx=0., 
	    const real & shifty=0., 
	    const real & shiftz=0. );

  // specify end conditions for the spline
  int setEndConditions( const EndCondition & condition, const RealArray & endValues=Overture::nullRealArray() );

  int setParameterizationType(const ParameterizationType & type);

  // supply a parameterization
  int setParameterization(const realArray & s );
  // get the current parameterization
  const realArray & getParameterization() const;
  
  // restrict the domain interval:
  int setDomainInterval(const real & rStart=0., const real & rEnd=1.);

  void setIsPeriodic( const int axis, const periodicType isPeriodic );


  // supply points for a 1d spline parameterized by index
  int setPoints( const realArray & x );
  // Supply points on the 2D spline (parameterized by arclength)
  int setPoints( const realArray & x, const realArray & y );
  // Supply points on the 3D spline (parameterized by arclength)
  int setPoints( const realArray & x, const realArray & y, const realArray & z );

  // create a shape preserving (monotone) spline or not
  int setShapePreserving( const bool trueOrFalse = TRUE );
  
  // specify a constant tension
  int setTension( const real & tension );

  // use the old spline routines from FMM:
  int useOldSpline( const bool & trueOrFalse = TRUE );
  
  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return SplineMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:
  void initialize();

  aString className;
  int numberOfSplinePoints;
  realArray s,knots,bcd,xp,sigma;
  bool useTensionSplines;
  bool initialized;
  bool pointAddedForPeriodicity;
  bool shapePreserving;
  EndCondition endCondition;
  real tension;
  RealArray bcValue;  // values for derivative boundary conditions.

  periodicType splineIsPeriodic;  // periodicity of underlying spline, independent of the reparameterization.
  real arcLengthWeight, curvatureWeight, rStart,rEnd;
  ParameterizationType parameterizationType;

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((SplineMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((SplineMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new SplineMapping(*this, ct); }

};


#endif  
