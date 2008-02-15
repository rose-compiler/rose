#ifndef ORTHOGRAPHIC_TRANSFORM_H
#define ORTHOGRAPHIC_TRANSFORM_H

#include "Mapping.h"
  
class OrthographicTransform : public Mapping
//---------------------------------------------------------------------
//  Define an orthographic Transformation which can be used to remove
//  polar singularities
//---------------------------------------------------------------------
{
 public:

  enum Pole
  {
    southPole=-1,
    northPole=+1
  };
  
  OrthographicTransform( const real sa=1., const real sb=1., const Pole pole=northPole );

  // Copy constructor is deep by default
  OrthographicTransform( const OrthographicTransform &, const CopyType copyType=DEEP );

  ~OrthographicTransform();

  OrthographicTransform & operator =( const OrthographicTransform & X0 );

  int setPole( const Pole & pole );

  int setSize( const int & sa, const int & sb );
  
  int setAngularAxis( const int & tAxis );   // angular axis is axis2 (default) or axis1
  
  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return OrthographicTransform::className; }

  int update( MappingInformation & mapInfo ) ;

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((OrthographicTransform &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((OrthographicTransform &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new OrthographicTransform(*this, ct); }

private:
  aString className;
  real sa,sb;
  int pole;  // reparameterize north pole (pole=1) or south pole (pole=-1)
  realArray s,a;  // for local computations
  int tAxis;      // angular (tangential axis) axis

};


#endif  


