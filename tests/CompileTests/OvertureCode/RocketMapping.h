#ifndef ROCKET_MAPPING_H
#define ROCKET_MAPPING_H 

#include "SplineMapping.h"
  
//-------------------------------------------------------------
//  Define cross-sections for rocket related geometries
//-------------------------------------------------------------
class RocketMapping : public SplineMapping
{

public:

  enum RocketTypeEnum
  {
    slot,
    star,
    circle
  };
  
  RocketMapping(const int & rangeDimension=2 );

  // Copy constructor is deep by default
  RocketMapping( const RocketMapping &, const CopyType copyType=DEEP );

  ~RocketMapping();

  RocketMapping & operator =( const RocketMapping & X0 );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return RocketMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:

  aString className;
  RocketTypeEnum rocketType;
  
  int numVertex;
  real elementSize;
  real normalDistance;
  real zValue;

  // for slot
  real slotWidth;
  int numPoints;

  // for star:
  real innerRadius, outerRadius;
  real innerFilletRadius, outerFilletRadius;

  // for circle:
  real radius;

  // compute spline points for a slot
  int computePoints();
  int computeSlotPoints();
  int computeStarPoints();
  int computeCirclePoints();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((RocketMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((RocketMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new RocketMapping(*this, ct); }

};


#endif  
