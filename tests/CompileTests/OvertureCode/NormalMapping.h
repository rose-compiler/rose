#ifndef NORMAL_MAPPING_H
#define NORMAL_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a Mapping by extending normals from another Mapping
//-------------------------------------------------------------
class NormalMapping : public Mapping
{
private:
  aString className;
  Mapping *surface;   // here is the surface that we extend normals from
  real normalDistance;

public:

  NormalMapping();
  NormalMapping(Mapping & surface, const real & normalDistance=.5 );

  // Copy constructor is deep by default
  NormalMapping( const NormalMapping &, const CopyType copyType=DEEP );

  ~NormalMapping();

  NormalMapping & operator =( const NormalMapping & X0 );

  void setReferenceSurface( Mapping & surface );
  void setNormalDistance( real distance );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return NormalMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:

  void initialize();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((NormalMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((NormalMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new NormalMapping(*this, ct); }

};


#endif  
