#ifndef REVOLUTION_MAPPING_H
#define REVOLUTION_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a Mapping by revolving another mapping about a line
//-------------------------------------------------------------
class RevolutionMapping : public Mapping
{

public:

  RevolutionMapping();
  //
  // This constructor takes a mapping to revolve plus option parameters
  //
  RevolutionMapping(Mapping & revolutionary, 
		    const real startAngle=0., 
		    const real endAngle=1.,
		    const RealArray & lineOrigin=Overture::nullRealArray(),
		    const RealArray & lineTangent=Overture::nullRealArray()
		    );

  // Copy constructor is deep by default
  RevolutionMapping( const RevolutionMapping &, const CopyType copyType=DEEP );

  ~RevolutionMapping();

  RevolutionMapping & operator =( const RevolutionMapping & X0 );
  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  int setLineOfRevolution(const RealArray & lineOrigin,
                          const RealArray & lineTangent );
  int setParameterAxes( const int & revAxis1, const int & revAxis2, const int & revAxis3 );
  int setRevolutionary(Mapping & revolutionary );
  int setRevolutionAngle( const real startAngle=0., const real endAngle=1. );
  int getRevolutionAngle( real & startAngle_, real & endAngle_ );
  
  void setBounds();

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return RevolutionMapping::className; }

  int update( MappingInformation & mapInfo ) ;

private:
  aString className;
  Mapping *revolutionary;    // here is the mapping that we revolve
  real startAngle,endAngle;     // revolve between these angles
  RealArray lineOrigin;  // revolve about a line that passes through this point
  RealArray lineTangent; // revolve about a line with this as tangent
  real signForTangent;
  int revAxis1,revAxis2,revAxis3;  // axes used, a permutation of (0,1,2)
  bool uninitialized;

  realArray a,b,c,y,yr,theta,ct,st,ar,br,cr;  // local arrays, not need to save with get/put

  void initialize();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((RevolutionMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((RevolutionMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType copyType = DEEP ) const
      { return ::new RevolutionMapping(*this, copyType); }

};


#endif  
