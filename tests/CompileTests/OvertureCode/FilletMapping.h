#ifndef FILLET_MAPPING_H
#define FILLET_MAPPING_H 

#include "IntersectionMapping.h"
  
//-------------------------------------------------------------
//  Define a fillet mapping that creates a smooth surface in
//  the reagion where two curves (surfaces) intersect
//-------------------------------------------------------------
class FilletMapping : public Mapping
{
public:

  enum JoinTypeEnum
  {
    fillet,
    collar
  };

  enum FilletTypeEnum    // There are two types of fillets
  {
    parametric=0,
    nonParametric
  };

  enum OffsetTypeEnum
  {
    hyperbolicOffset,
    offsetCurve
  };

  FilletMapping();
  FilletMapping(Mapping & curve1, 
		Mapping & curve2);

  // Copy constructor is deep by default
  FilletMapping( const FilletMapping &, const CopyType copyType=DEEP );

  ~FilletMapping();

  FilletMapping & operator =( const FilletMapping & X0 );

  int setCurves(Mapping & curve1, 
		Mapping & curve2);

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return FilletMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:
  aString className;
  Mapping *curve[2], *blendingFunction;  
  bool uninitialized;
  IntersectionMapping inter;
  int numberOfIntersections;       // number of intersection points/curves
  int intersectionToUse;           // make a fillet at this intersection
  bool intersectionFound;          // true if an intersection curve has been found.
  real filletWidth, filletOverlap; // width of fillet, amount of overlap onto each curve/surface
  int orient;                      // defines the orientation
  RealArray sc;                     // holds coefficients in s_i(r) functions
  real blendingFactor;              // determines the sharpeness of the blend.
  // for intersection of curves:
  realArray rIntersect[2],xIntersect;
  bool newCurves;
  Mapping *s[2];                 // for parameterizing a 3D fillet

  JoinTypeEnum joinType;
  FilletTypeEnum filletType;
  OffsetTypeEnum offsetType[2];
  
  int setup();
  int setupForNewCurves();
  void initialize();
  
 private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((FilletMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((FilletMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new FilletMapping(*this, ct); }

};


#endif  
