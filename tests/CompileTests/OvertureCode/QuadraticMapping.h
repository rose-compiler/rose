#ifndef QUADRATIC_MAPPING_H
#define QUADRATIC_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a quadratic curve or surface
//
//-------------------------------------------------------------
class QuadraticMapping : public Mapping
{

public:

  enum QuadraticOption
  {
    parabola,
    hyperbola
  };

  QuadraticMapping();
  // Copy constructor is deep by default
  QuadraticMapping( const QuadraticMapping &, const CopyType copyType=DEEP );

  ~QuadraticMapping();

  QuadraticMapping & operator =( const QuadraticMapping & X0 );

  int chooseQuadratic( QuadraticOption option, int rangeDimension=2 );
  int setParameters(real c0x, 
		    real c1x,
		    real c0y,
		    real c1y,
		    real a00,
		    real a10, 
		    real a01, 
		    real a20, 
		    real a11, 
		    real a02,
                    real signForHyperbola = 1. );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return QuadraticMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:
  aString className;

  QuadraticOption quadraticOption;
  
  real a[3][3],c[2][2];  // quadratic parameters
  real signForHyperbola;
  
  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((QuadraticMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((QuadraticMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new QuadraticMapping(*this, ct); }

};


#endif  
