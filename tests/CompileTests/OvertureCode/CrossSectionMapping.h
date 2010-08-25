#ifndef CROSS_SECTION_MAPPING_H
#define CROSS_SECTION_MAPPING_H

#include "Mapping.h"
class SplineMapping;

class CrossSectionMapping : public Mapping{
//-----------------------------------------------------------------------------------
//  Here is a derived class to define a Cylindrical Surface in 3D
//
//   The surface is defined in terms of cylindrical coordinates
//
//              (s,theta/(2pi),r) :   s in [0,1],  theta in [0,2pi], r in [0,1]
//-----------------------------------------------------------------------------------


 public:

  enum CrossSectionTypes
  {
    general,
    ellipse,
    banana,
    joukowsky,
    m6,
    numberOfCrossSectionTypes   // this entry counts the number of items in this list
  };

  enum Parameterization
  {
    arcLength=0,
    index,
    userDefined
  };
  
  enum AxialApproximation
  {
    linear=0,
    cubic
  };

  CrossSectionMapping(const real startS=0.,                // staring value for "s"
		      const real endS=1.,                  // ending value for "s"
		      const real startAngle=0.,
		      const real endAngle=1.,
		      const real innerRadius=1., 
		      const real outerRadius=1.5,
		      const real x0=0., 
		      const real y0=0., 
		      const real z0=0., 
		      const real length=1.,
		      const int domainDimension=3 );

  // Copy constructor is deep by default
  CrossSectionMapping( const CrossSectionMapping & map, const CopyType copyType=DEEP );

  ~CrossSectionMapping();

  CrossSectionMapping & operator =( const CrossSectionMapping & X );

  int setCrossSectionType(CrossSectionTypes type);

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters() );

  void basicInverse(const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
                    MappingParameters & params = Overture::nullMappingParameters());

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );

  aString getClassName() const;

  int update( MappingInformation & mapInfo ) ;

 protected:
  aString className;

  int tAxis,sAxis;
  CrossSectionTypes crossSectionType;
  int numberOfCrossSections;
  Mapping **crossSection;   // array of pointers to mappings that define the cross-sections
  AxialApproximation axialApproximation;
  Parameterization parameterization;
  realArray s;              // hold parameterization (by default arclength)
  Mapping *parameterMap;    // holds spline for non-index parameterization
  IntegerArray *ia;         // ia[cs] : indirect addressing for evaluating the mapping
  realArray centroid;       // centroid of each cross-section, xc0(axis,cs)
  realArray csRadius;       // "radius" of each cross section is csRadius(cs)
  SplineMapping *radiusSpline;
  real polarSingularityFactor;

  // parameters for built-in cross section types
  real innerRadius,outerRadius,x0,y0,z0,length,startAngle,endAngle,a,b,c,startS,endS;
  real joukowskyDelta,joukowskyD,joukowskyA,joukowskyLength,joukowskyBeta;
  
  // evaluate cross section cs (also evaluate ghost cross-sections cs=-1 and cs=numberOfCrossSections.)
  int crossSectionMap( const int & cs, const realArray & r, realArray & x, realArray & xr);

  void mapGeneral( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
                   MappingParameters & params=Overture::nullMappingParameters() );
  void mapGeneralWithPolarSingularity( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
                                       MappingParameters & params=Overture::nullMappingParameters() );
  void mapBuiltin( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
                   MappingParameters & params=Overture::nullMappingParameters() );

  int initialize();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((CrossSectionMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((CrossSectionMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new CrossSectionMapping(*this, ct); }
};

#endif
