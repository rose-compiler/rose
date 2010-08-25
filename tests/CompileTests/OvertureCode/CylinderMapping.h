#ifndef CYLINDER_MAPPING_H
#define CYLINDER_MAPPING_H

#include "Mapping.h"

class CylinderMapping : public Mapping{
//-------------------------------------------------------------
//  Here is a derived class to define a Cylindrical Surface in 3D
//-------------------------------------------------------------


 public:
  real innerRadius,outerRadius,x0,y0,z0,startAngle,endAngle,startAxis,endAxis;

  CylinderMapping(const real & startAngle=0.,
		  const real & endAngle=1.,
		  const real & startAxis=-1.,
		  const real & endAxis=+1.,
		  const real & innerRadius=1., 
		  const real & outerRadius=1.5,
		  const real & x0=0., 
		  const real & y0=0., 
		  const real & z0=0., 
		  const int & domainDimension=3,
		  const int & cylAxis1 = axis1,
		  const int & cylAxis2 = axis2,
		  const int & cylAxis3 = axis3  );

  // Copy constructor is deep by default
  CylinderMapping( const CylinderMapping & map, const CopyType copyType=DEEP );

  ~CylinderMapping();

  CylinderMapping & operator =( const CylinderMapping & X );

  int setAngle(const real & startAngle=0., const real & endAngle=1. );
  int setAxis(const real & startAxis=-1., const real & endAxis=+1. );
  int setOrientation( const int & cylAxis1=axis1,  const int & cylAxis2=axis2,  const int & cylAxis3=axis3 );
  int setOrigin(const real & x0=0., const real & y0=0., const real & z0=0. );
  int setRadius(const real & innerRadius=1., const real & outerRadius=1.5 );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params=Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );

  aString getClassName() const;

  int update( MappingInformation & mapInfo ) ;

 protected:
  int checkAxes();
  int updatePeriodVector();
  
 private:
  aString className;
  realArray radius,angle;
  int cylAxis1, cylAxis2, cylAxis3;

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((CylinderMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((CylinderMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new CylinderMapping(*this, ct); }
};

#endif
