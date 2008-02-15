#ifndef STRETCHED_SQUARE_H
#define STRETCHED_SQUARE_H "StretchedSquare.h"

#include "StretchMapping.h"
  
//-------------------------------------------------------------------------
//  Here is a derived class to define a Stretched Unit Square or Unit Cube
//
//  This stretched unit square (cube) can be composed with another Mapping
//  in order to stretch grid lines on the latter Mapping
//-------------------------------------------------------------------------
class StretchedSquare : public Mapping
{
public:

  StretchedSquare( const int & domainDimension=2 );

  // Copy constructor is deep by default
  StretchedSquare( const StretchedSquare &, const CopyType copyType=DEEP );

  ~StretchedSquare();

  StretchedSquare & operator =( const StretchedSquare & X0 );

  // return a reference to the stretching function along a given coordinate direction
  StretchMapping & stretchFunction( const int & axis = 0 );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return StretchedSquare::className; }

  int update( MappingInformation & mapInfo ) ;

protected:
  aString className;
  StretchMapping stretch[3];

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((StretchedSquare &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((StretchedSquare &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new StretchedSquare(*this, ct); }

};


#endif   // STRETCHEDSQUARE_H


