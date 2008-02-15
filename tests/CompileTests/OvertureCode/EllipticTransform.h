#ifndef ELLIPTIC_TRANSFORM
#define ELLIPTIC_TRANSFORM

#include "Mapping.h"
#include "MappedGridOperators.h"

// forward declarations:
class GenericGraphicsInterface; 
class GraphicsParameters;       
class ComposeMapping;           
class DataPointMapping;

// extern GraphicsParameters Overture::defaultGraphicsParameters();
class EllipticGridGenerator;

class EllipticTransform : public Mapping
{
//----------------------------------------------------------------
//  Smooth out a mapping with Elliptic Grid Generation
//  -------------------------------------------------
//
//----------------------------------------------------------------

 public:

  EllipticTransform();

  // Copy constructor is deep by default
  EllipticTransform( const EllipticTransform &, const CopyType copyType=DEEP );

  ~EllipticTransform();

  EllipticTransform & operator =( const EllipticTransform & X );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void inverseMap( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
                   MappingParameters & params =Overture::nullMappingParameters() );

  void generateGrid(GenericGraphicsInterface *gi=NULL, 
                    GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

  void periodicUpdate(RealArray &x, RealArray xo, RealArray pSrc, RealArray qSrc);
  void bcUpdate(RealArray &x,RealArray xo,RealArray &pSrc, int iteration);
  void initialize();

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return EllipticTransform::className; }

  int update( MappingInformation & mapInfo ) ;


 protected:

  int setup();
  void resetDataPointMapping( realArray & x,Index I, Index J, Index K);

  aString className;
  Mapping *userMap;             // original mapping before elliptic grid generation
  ComposeMapping *compose;      // holds the composite map if project==TRUE
  DataPointMapping *dpm;        // This holds the elliptic grid if project==FALSE or
                                // the reparameterization map if project==TRUE
  bool ellipticGridDefined;     // TRUE when the grid has been generated

  RealArray xe;    // xe holds the grid points of the elliptic transform

  EllipticGridGenerator *ellipticGridGenerator;


 private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
  virtual ReferenceCounting& operator=(const ReferenceCounting& x)
    { return operator=((EllipticTransform &)x); }
  virtual void reference( const ReferenceCounting& x) 
    { reference((EllipticTransform &)x); }     
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new EllipticTransform(*this, ct); }
};


#endif
