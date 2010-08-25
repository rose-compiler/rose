#ifndef ANNULUS_MAPPING_H
#define ANNULUS_MAPPING_H

#include "Mapping.h"

//-------------------------------------------------------------
//  Here is a derived class to define a Annulus in 2D
//
//-------------------------------------------------------------
class AnnulusMapping : public Mapping
{

 private:
  aString className;
  realArray angle;
  realArray radius;

 public:
  real x0,y0,innerRadius,outerRadius,startAngle,endAngle,zLevel;

  AnnulusMapping(const real innerRadius0=.5, 
                 const real outerRadius0=1., 
		 const real x0=0., 
		 const real y0=0., 
		 const real startAngle=0.,
                 const real endAngle=1.  );



  // Copy constructor is deep by default
  AnnulusMapping( const AnnulusMapping &, const CopyType copyType=DEEP );

  ~AnnulusMapping();

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters());

  void basicInverse( const realArray & x, realArray & r, realArray & rx= Overture::nullRealDistributedArray(), 
           MappingParameters & params =Overture::nullMappingParameters());

  AnnulusMapping & operator =( const AnnulusMapping & X0 );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  int setRadii(const real & innerRadius=.5, 
               const real & outerRadius=1. );
  int setOrigin(const real & x0=0., 
		const real & y0=0., 
		const real & z0=0. );
  int setAngleBounds(const real & startAngle=0., 
                     const real & endAngle=1. );

  Mapping *make( const aString & mappingClassName );

  aString getClassName() const;

  int update( MappingInformation & mapInfo );  

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=( (AnnulusMapping &)x); }
    virtual void reference( const ReferenceCounting& x)
      { reference( (AnnulusMapping &)x); }
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new AnnulusMapping(*this, ct); }
};

#endif
