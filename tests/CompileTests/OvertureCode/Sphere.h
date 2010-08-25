#ifndef SPHERE_H
#define SPHERE_H "Sphere.h"

#include "Mapping.h"

//-------------------------------------------------------------
//  Define a sphere in polar coordinates
//
//   map  (r1,r2,r3) = (phi/Pi, theta/twoPi, r ) -> (x,y,z)
//-------------------------------------------------------------
class SphereMapping : public Mapping
{
private:
 aString className;
 real x0,y0,z0,innerRadius,outerRadius,startTheta,endTheta,startPhi,endPhi;
 realArray radius;

public:

SphereMapping(const real & innerRadius=.5, 
	      const real & outerRadius=1.,
	      const real & x0=0., 
              const real & y0=0., 
	      const real & z0=0.,
              const real & startTheta=0.,       // initial value for "theta", in [0,1]
              const real & endTheta=1.,         // final value for "theta", in [0,1]
              const real & startPhi=0.,         // initial value for "phi", in [0,1]
              const real & endPhi=1.            // final value for "phi", in [0,1] 
               );


  // Copy constructor is deep by default
  SphereMapping( const SphereMapping &, const CopyType copyType=DEEP );

  ~SphereMapping();

  SphereMapping & operator =( const SphereMapping & X );

  int setOrigin(const real & x0_ =.0, 
		const real & y0_ =.0, 
		const real & z0_ =.0);
  int setPhi(const real & startPhi_ =.0,
             const real & endPhi_   = 1.);

  int setRadii(const real & innerRadius_ =.5, 
               const real & outerRadius_ =1.);

  int setTheta( const real & startTheta_ =.0,
		const real & endTheta_   =1. );
  
  void map(const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
           MappingParameters & params = Overture::nullMappingParameters() );

  void basicInverse(const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
                    MappingParameters & params = Overture::nullMappingParameters());

  int outside( const realArray & x ); // false means no or don't no

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return SphereMapping::className; }

  int update( MappingInformation & mapInfo );  

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((SphereMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((SphereMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new SphereMapping(*this, ct); }
};


#endif   // SPHERE_H
