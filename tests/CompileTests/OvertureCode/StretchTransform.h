#ifndef STRETCH_TRANSFORM
#define STRETCH_TRANSFORM

#include "ComposeMapping.h"
#include "StretchedSquare.h"

class StretchTransform : public ComposeMapping{
//----------------------------------------------------------------
//  Stretch a Mapping along the Coordinate Directions 
//  -------------------------------------------------
//
//  Compose a Mapping with a StretchedSquare
//----------------------------------------------------------------
 private:
  aString className;
  StretchedSquare *stretchedSquare;        // this mapping is use to stretch
 public:

  StretchTransform( );

  void setMapping( Mapping & map );  // set the mapping for the stretch tranform
  StretchedSquare & getStretchedSquare();

  void reinitialize(); // call this function if you change the StretchedSquare
  
  // Copy constructor is deep by default
  StretchTransform( const StretchTransform &, const CopyType copyType=DEEP );

  ~StretchTransform();

  StretchTransform & operator =( const StretchTransform & X );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return StretchTransform::className; }

  int update( MappingInformation & mapInfo ) ;


 protected:
   int setMappingProperties();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((StretchTransform &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((StretchTransform &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new StretchTransform(*this, ct); }
  };


#endif
