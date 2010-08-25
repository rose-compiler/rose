#ifndef COMPOSE_MAPPING_H
#define COMPOSE_MAPPING_H "ComposeMapping.h"

#include <A++.h>
#include <string.h>
#include <assert.h>
#include "Mapping.h"
#include "Inverse.h"
#include "MappingRC.h"

class ComposeMapping : public Mapping{
//----------------------------------------------------------------
//       Compose two Mapping functions
//       -----------------------------
// Constructor example:
//   ComposeMapping mapc( mapa,mapb );
// ...means
//     mapc <-  (mapb o mapa)
//  mapc means to apply mapa followed by mapb
//----------------------------------------------------------------
 public:

  // keep reference counted versions of the mappings that we compose
  MappingRC map1;
  MappingRC map2;

  ComposeMapping( );

  //
  // Constructor specifying the two mappings to compose, mapb o mapa
  //
  ComposeMapping( Mapping & mapa, Mapping & mapb );

  //
  // specify the two mappings to compose, mapb o mapa
  //
  void setMappings( Mapping & mapa, Mapping & mapb );

  // Copy constructor is deep by default
  ComposeMapping( const ComposeMapping &, const CopyType copyType=DEEP );

  ~ComposeMapping();

  ComposeMapping & operator =( const ComposeMapping & X );

  const realArray& getGrid(MappingParameters & params=Overture::nullMappingParameters());

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return ComposeMapping::className; }

  virtual void useRobustInverse(const bool trueOrFalse=TRUE );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters());

  void inverseMap( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
                   MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx= Overture::nullRealDistributedArray(), 
                     MappingParameters & params=Overture::nullMappingParameters());

  // Use default Mapping inverse instead of optimized inverse defined by this mapping
  int useDefaultMappingInverse(bool trueOrFalse = TRUE );

  virtual real sizeOf(FILE *file = NULL ) const;

  virtual int update( MappingInformation & mapInfo ) ;


 private:
  aString className;
  Range Rr,Ry,Rx;

 protected:

//  ApproximateGlobalInverse *approximateGlobalInverse;
//  ExactLocalInverse *exactLocalInverse;
//  MappingWorkSpace workSpace;  // work space for inverse
  bool useDefaultInverse;

  private:

   void setup( Mapping & mapa, Mapping & mapb );

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((ComposeMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((ComposeMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new ComposeMapping(*this, ct); }
  };


#endif // COMPOSE_MAPPING_H

