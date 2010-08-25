#ifndef RESTRICTION_H
#define RESTRICTION_H "Restriction.h"

#include "Mapping.h"
class ReparameterizationTransform;
  
//-------------------------------------------------------------
//  This mapping restricts parameter space to a sub-rectangle
//
//  For example, in 2D it maps [0,1]x[0,1] -> [ra,rb]x[sa,sb]
// 
//-------------------------------------------------------------
class RestrictionMapping : public Mapping
{
  friend ReparameterizationTransform;
  
private:
  aString className;
  real ra,rb,sa,sb,ta,tb;
  int spaceIsPeriodic[3];   // true if the space we are reparameterizing is periodic.

public:

  RestrictionMapping(const real ra=0., 
                     const real rb=1., 
		     const real sa=0.,
		     const real sb=1.,
		     const real ta=0.,
		     const real tb=1.,
                     const int dimension=2,
                     Mapping *restrictedMapping=NULL );    // 2D by default

  // Copy constructor is deep by default
  RestrictionMapping( const RestrictionMapping &, const CopyType copyType=DEEP );

  ~RestrictionMapping();

  RestrictionMapping & operator =( const RestrictionMapping & X0 );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return RestrictionMapping::className; }

  int update( MappingInformation & mapInfo ) ;

  // scale the current bounds
  int scaleBounds(const real ra=0., 
		  const real rb=1., 
		  const real sa=0.,
		  const real sb=1.,
		  const real ta=0.,
		  const real tb=1. );
  // set absolute bounds 
  int setBounds(const real ra=0., 
		const real rb=1., 
		const real sa=0.,
		const real sb=1.,
		const real ta=0.,
		const real tb=1. );

  int getBounds(real & ra, real & rb, real & sa, real & sb, real & ta, real & tb ) const;

  // indicate whether the mapping being reparameterized is periodic
  int setSpaceIsPeriodic( int axis, bool trueOrFalse = true );
  
  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((RestrictionMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((RestrictionMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new RestrictionMapping(*this, ct); }

};


#endif   // RESTRICTION_H


