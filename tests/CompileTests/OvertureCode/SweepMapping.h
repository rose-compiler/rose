#ifndef SWEEP_MAPPING_H
#define SWEEP_MAPPING_H

#include "Mapping.h"
#include "SplineMapping.h"
#include "MatrixTransform.h"

class SweepMapping : public Mapping
{

 public:

  enum CenteringOptionsEnum
  {
    useCenterOfSweepSurface,
    useCenterOfSweepCurve
  };

  enum SweepTypeEnum
  {
    sweep,
    extrude,
    tabulatedCylinder
  };

  SweepMapping(Mapping *sweepmap=NULL,
               Mapping *dirsweepmap=NULL,
	       Mapping *scale=NULL,
	       const int domainDimension=3 );

  // Copy constructor is deep by default
  SweepMapping( const SweepMapping & map, const CopyType copyType=DEEP );

  ~SweepMapping();

  SweepMapping & operator =( const SweepMapping & X );

  void map( const realArray & r, realArray & x=Overture::nullRealDistributedArray(), realArray & xr = Overture::nullRealDistributedArray(),
         MappingParameters & params=Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  int setCentering( CenteringOptionsEnum centeringOption );  // specify the centering

  // build an extruded mapping
  int setExtrudeBounds( real za=0., real zb=1. );

// build a tabulated cylinder
  int setStraightLine(real lx=0., real ly=0., real lz=1.);
  
  void setScale(Mapping *scale);                             // specify a Mapping for scaling
  void setSweepCurve(Mapping *dirsweepmap);                  // specify the sweep curve
  void setSweepSurface(Mapping *sweepmap);                   // specify the sweep surface

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );

  aString getClassName() const;

  int update( MappingInformation & mapInfo ) ;

 protected:
  RealArray straightLine;
  
  aString className;
  Mapping *sweepMap, *dirSweepMap;

  SplineMapping *rowSpline0, *rowSpline1, *rowSpline2;
  Mapping *scaleSpline;

  real center[3];
  void findRowSplines();
  CenteringOptionsEnum centeringOption;
  SweepTypeEnum sweepType;
  real za,zb;

  int initialize();
  int setMappingProperties();
  void crossProduct(real *v1, real *v2, real *v3);
  real dotProduct(real *v1, real *v2);

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((SweepMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((SweepMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new SweepMapping(*this, ct); }
};

#endif
