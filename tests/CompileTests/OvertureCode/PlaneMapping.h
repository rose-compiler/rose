#ifndef PLANE_MAPPING
#define PLANE_MAPPING

#include "Mapping.h"

class PlaneMapping : public Mapping{
//-------------------------------------------------------------
//  Here is a Mapping class to define a plane in 3D 
//-------------------------------------------------------------
 public:
  aString className;
  real xa,ya,za;
  RealArray vector1, vector2;

  PlaneMapping(const real & x1=0., const real & y1=0., const real & z1=0.,
               const real & x2=1., const real & y2=0., const real & z2=0.,
               const real & x3=0., const real & y3=1., const real & z3=0.);


  // Copy constructor is deep by default
  PlaneMapping( const PlaneMapping &, const CopyType copyType=DEEP );

  ~PlaneMapping();

  PlaneMapping & operator =( const PlaneMapping & X );

  int setPoints(const real & x1=0., const real & y1=0., const real & z1=0.,
		const real & x2=1., const real & y2=0., const real & z2=0.,
		const real & x3=0., const real & y3=1., const real & z3=0.);

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  aString getClassName() const { return PlaneMapping::className; }

  Mapping* make( const aString & mappingClassName );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters());

  void basicInverse( const realArray & x, realArray & r, realArray & rx= Overture::nullRealDistributedArray(), 
                     MappingParameters & params =Overture::nullMappingParameters());

  int update( MappingInformation & mapInfo ) ;

 protected:
  real v1DotV1, v1DotV2, v2DotV2;
  realArray v1DotX, v2DotX;
  
  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((PlaneMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((PlaneMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new PlaneMapping(*this, ct); }
};

#endif
