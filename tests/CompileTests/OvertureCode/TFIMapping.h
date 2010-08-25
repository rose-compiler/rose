// -*- c++ -*-
#ifndef TFI_MAPPING_H
#define TFI_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a Mapping by Transfinite-Interpolation (Coon's patch)
//-------------------------------------------------------------
class TFIMapping : public Mapping
{

public:

  enum InterpolationType
  {
    linear,
    hermite,
    blend
  };

  // supply 0, 2, 4 or 6 curves.
  TFIMapping(Mapping *left    =NULL,   // r_1=0
             Mapping *right   =NULL,   // r_1=1
	     Mapping *bottom  =NULL,   // r_2=0
	     Mapping *top     =NULL,   // r_2=1
	     Mapping *front   =NULL,   // r_3=0
	     Mapping *back    =NULL);  // r_3=1

  // Copy constructor is deep by default
  TFIMapping( const TFIMapping &, const CopyType copyType=DEEP );

  ~TFIMapping();

  TFIMapping & operator =( const TFIMapping & X0 );

  // supply 0, 2, 4 or 6 curves.
  int setSides(Mapping *left    =NULL,   // r_1=0
               Mapping *right   =NULL,   // r_1=1
	       Mapping *bottom  =NULL,   // r_2=0
	       Mapping *top     =NULL,   // r_2=1
	       Mapping *front   =NULL,   // r_3=0
	       Mapping *back    =NULL);  // r_3=1

  // set the interpolation type along each axis
  int setInterpolationType(const InterpolationType & direction1, 
                           const InterpolationType & direction2 =linear, 
			   const InterpolationType & direction3 =linear);

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  int flipper();  // try to flip the curve parameterizations to make the mapping non-singular.

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return TFIMapping::className; }

  int update( MappingInformation & mapInfo ) ;

  private:

  aString className;
  int numberOfSidesSpecified;  // can be 2,4 or 6
  InterpolationType interpolationType[3];   // type of interpolation along each dimension 

  realArray corner;
  IntegerArray flip;
  
  //**** should referece count these so we can delete them *****************************************
  Mapping *curve[2][3];         // These curves are the sides of the TFI mapping, at most 6 
  Mapping *blendingFunction;   // general blending function

  // member function to initialize some of the data, especially the Corner array
  void initialize();

  int setMappingProperties();

  int checkEdge( const int & sideCurve1, const int & axisCurve1, const int & side1, const int & dir1, 
                 const int & sideCurve2, const int & axisCurve2, const int & side2, const int & dir2 );



  //-------------FIXING SGI COMPILER BUG
  void mapNumberOfSides2( const realArray & r, realArray & x, 
			  realArray & xr = Overture::nullRealDistributedArray(),
			  MappingParameters & params =Overture::nullMappingParameters() );

  void mapNumberOfSides4AndDim2( 
	      const realArray & r, realArray & x, 
	      realArray & xr = Overture::nullRealDistributedArray(),
	      MappingParameters & params =Overture::nullMappingParameters() );

  void mapNumberOfSides4AndDim3( 
	      const realArray & r, realArray & x, 
	      realArray & xr = Overture::nullRealDistributedArray(),
	      MappingParameters & params =Overture::nullMappingParameters() );

  void mapNumberOfSides6( 
	      const realArray & r, realArray & x, 
	      realArray & xr = Overture::nullRealDistributedArray(),
	      MappingParameters & params =Overture::nullMappingParameters() );
  


  
  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((TFIMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((TFIMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new TFIMapping(*this, ct); }

};


#endif  
