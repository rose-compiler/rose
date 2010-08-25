#ifndef CIRCLE_MAPPING_H
#define CIRCLE_MAPPING_H

#include "Mapping.h"

//-------------------------------------------------------------
//  Define a 2D Circle or Ellipse
//-------------------------------------------------------------
class CircleMapping : public Mapping
{
 public:
  aString className;
  real a,b,xa,ya,za;
  real startTheta, endTheta;

  // define a circle or ellipse in 2D, minor axis a, major axis b, angle from startTheta to endTheta
  CircleMapping(const real & x0=0., 
		const real & y0=0., 
		const real & a=1., 
		const real & b=-1.,
                const real & startTheta=0., 
		const real & endTheta=1.) ;
  // define a circle or ellipse in 3D
  CircleMapping(const real & x0, 
		const real & y0, 
		const real & z0, 
		const real & a, 
		const real & b, 
                const real & startTheta, 
		const real & endTheta ) ;

  // Copy constructor is deep by default
  CircleMapping( const CircleMapping & map, const CopyType copyType=DEEP );

  ~CircleMapping();

  CircleMapping & operator =( const CircleMapping & x );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters());

  void basicInverse(const realArray & x, realArray & r, realArray & rx= Overture::nullRealDistributedArray(), 
                     MappingParameters & params =Overture::nullMappingParameters());

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );

  aString getClassName() const;

  int update( MappingInformation & mapInfo ) ;

  protected:

  void initialize(const real & x0, const real & y0, const real & z0, const real & a, const real & b, 
                  const real & startTheta, const real & endTheta ) ;
  private:
  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((CircleMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((CircleMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new CircleMapping(*this, ct); }
};

#endif
