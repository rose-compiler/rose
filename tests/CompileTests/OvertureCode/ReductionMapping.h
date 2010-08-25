#ifndef REDUCTION_MAPPING_H
#define REDUCTION_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a new Mapping for the face or edge of another Mapping, thus
//  reducing the domain dimension.
//-------------------------------------------------------------
class ReductionMapping : public Mapping
{

public:

  ReductionMapping();
  ReductionMapping(Mapping & mapToReduce, 
                   const real & inactiveAxis1Value =0. ,
		   const real & inactiveAxis2Value =-1.,
		   const real & inactiveAxis3Value =-1. );
  ReductionMapping(Mapping & mapToReduce, 
                   const int & inactiveAxis,
		   const real & inactiveAxisValue );
  
  // Copy constructor is deep by default
  ReductionMapping( const ReductionMapping &, const CopyType copyType=DEEP );

  ~ReductionMapping();

  ReductionMapping & operator =( const ReductionMapping & X0 );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  int set(Mapping & mapToReduce, 
	  const real & inactiveAxis1Value =0. ,
	  const real & inactiveAxis2Value =-1.,
	  const real & inactiveAxis3Value =-1. );

  int set(Mapping & mapToReduce, 
	  const int & inactiveAxis,
	  const real & inactiveAxisValue );

  int setInActiveAxes( const real & inactiveAxis1Value =0.,
                       const real & inactiveAxis2Value =-1.,
		       const real & inactiveAxis3Value =-1.);
  
  int setInActiveAxes( const int & inactiveAxis,
                       const real & inactiveAxisValue );
  
  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return ReductionMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 private:
  aString className;
  Mapping *originalMap;   // here is the original Mapping
  int numberOfInActiveAxes;
  int activeAxis[3], inActiveAxis[3];
  real inActiveAxisValue[3];          // The fixed r-value for the inActiveAxis
  
  int setInActiveAxes( const real value[3] );
  int setMappingProperties();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((ReductionMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((ReductionMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new ReductionMapping(*this, ct); }

};


#endif  
