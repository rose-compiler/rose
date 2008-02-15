#ifndef SMOOTHEDPOLYGON_H
#define SMOOTHEDPOLYGON_H "SmoothedPolygon.h"

#include "MappingRC.h"

class SmoothedPolygon : public Mapping
{
//-------------------------------------------------------------
//  Here is a derived class to define a SmoothedPolygon in 2D
//-------------------------------------------------------------
private:
  aString className;
  bool mappingIsDefined;

  int numberOfVertices;
  RealArray vertex,arclength;
  int NumberOfRStretch;
  int userStretchedInT,correctCorners;
  RealArray bv,sab,r12b,sr,corner;
  int numberOfRStretch;
  real normalDistance;                    
  real zLevel;

  int ndiwk,ndrwk;
  IntegerArray iwk;
  RealArray rwk;

  // no need to save the following:
  IntegerArray iw;  
  RealArray w;  
  int ndi;
  int ndr;
  int nsm1;
  RealArray bx, by, ccor;

public:

  SmoothedPolygon();

  // Copy constructor is deep by default
  SmoothedPolygon( const SmoothedPolygon &, const CopyType copyType=DEEP );

  ~SmoothedPolygon();

  int update( MappingInformation & mapInfo );  

  SmoothedPolygon & operator =( const SmoothedPolygon & X );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters());

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return SmoothedPolygon::className; }

 protected:
  void setDefaultValues();
  void initialize();
  void assignAdoptions();


  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((SmoothedPolygon &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((SmoothedPolygon &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new SmoothedPolygon(*this, ct); }
};


#endif   // SMOOTHEDPOLYGON_H
