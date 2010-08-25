#ifndef SQUARE_MAPPING_H
#define SQUARE_MAPPING_H "Square.h"

#include "Mapping.h"
  
class SquareMapping : public Mapping
{
//-------------------------------------------------------------
//  Here is a derived class to define a square in 2D
//-------------------------------------------------------------
public:

  SquareMapping( const real xa=0., 
                 const real xb=1., 
		 const real ya=0.,
                 const real yb=1. );

  // Copy constructor is deep by default
  SquareMapping( const SquareMapping &, const CopyType copyType=DEEP );

  ~SquareMapping();

  SquareMapping & operator =( const SquareMapping & X0 );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  void setVertices(const real xa=0., 
                   const real xb=1., 
		   const real ya=0.,
		   const real yb=1.,
                   const real z=0. ); 

  real getVertices(real & xa, real & xb, real & ya, real & yb ) const; 

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return SquareMapping::className; }

  int update( MappingInformation & mapInfo ) ;

private:
  aString className;
  RealArray xyab;
  real xa,xb,ya,yb;
  real z;           // z level if 3D
  
  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((SquareMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((SquareMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new SquareMapping(*this, ct); }

};


#endif   // SQUARE_H


