#ifndef LINE_MAPPING_H
#define LINE_MAPPING_H "Line.h"

#include "Mapping.h"

class LineMapping : public Mapping
{
//-------------------------------------------------------------
//  Here is a derived class to define a Line in 1D, 2D or 3D
//-------------------------------------------------------------
private:
  aString className;
  real xa,xb,ya,yb,za,zb;

public:

  LineMapping(const real xa=0., 
              const real xb=1., 
              const int numberOfGridPoints=11);
  LineMapping(const real xa, const real ya, 
              const real xb, const real yb,
              const int numberOfGridPoints=11);
  LineMapping(const real xa, const real ya, const real za, 
              const real xb, const real yb, const real zb,
              const int numberOfGridPoints=11);


  // Copy constructor is deep by default
  LineMapping( const LineMapping &, const CopyType copyType=DEEP );

  ~LineMapping();

  LineMapping & operator =( const LineMapping & X0 );

  int getPoints( real & xa, real & xb ) const;
  int getPoints( real & xa, real & ya,
                 real & xb, real & yb ) const;
  int getPoints( real & xa, real & ya, real & za, 
                 real & xb, real & yb, real & zb ) const;


  int setPoints( const real & xa, const real & xb );
  int setPoints( const real & xa, const real & ya,
                 const real & xb, const real & yb );
  int setPoints( const real & xa, const real & ya, const real & za, 
                 const real & xb, const real & yb, const real & zb );

  void map(const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
           MappingParameters & params =Overture::nullMappingParameters());

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
           MappingParameters & params =Overture::nullMappingParameters());

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return LineMapping::className; }

  int update( MappingInformation & mapInfo ) ;

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((LineMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((LineMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new LineMapping(*this, ct); }

};


#endif   // LINE_MAPPING_H
