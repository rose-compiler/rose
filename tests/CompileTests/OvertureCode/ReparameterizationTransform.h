#ifndef REPARAMETERIZATION_TRANSFORM
#define REPARAMETERIZATION_TRANSFORM

#include "ComposeMapping.h"

class OrthographicTransform;

class ReparameterizationTransform : public ComposeMapping
{
//----------------------------------------------------------------
//  Reparameterize a Mapping in various ways
//  ----------------------------------------
//
//----------------------------------------------------------------
 public:

  enum ReparameterizationTypes
  {
    defaultReparameterization,
    orthographic,
    restriction,
    equidistribution
  };


 public:

  ReparameterizationTransform( );
  //
  // Constructor, supply a Mapping to reparameterize
  // It will replace multiple reparams with just one reparam if map is actually of this class
  ReparameterizationTransform(Mapping & map,     const ReparameterizationTypes type=defaultReparameterization );
  ReparameterizationTransform(MappingRC & mapRC, const ReparameterizationTypes type=defaultReparameterization );

  // Copy constructor is deep by default
  ReparameterizationTransform( const ReparameterizationTransform &, const CopyType copyType=DEEP );

  ~ReparameterizationTransform();

  ReparameterizationTransform & operator =( const ReparameterizationTransform & X );

  // set equidistribution parameterization parameters
  int setEquidistributionParameters(const real & arcLengthWeight=1., 
                                    const real & curvatureWeight=0.,
                                    const int & numberOfSmooths = 3 );
  
  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void inverseMap( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
                   MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return ReparameterizationTransform::className; }

  int update( MappingInformation & mapInfo ) ;

  // scale the current bounds for a restriction mapping
  int scaleBounds(const real ra=0., 
		  const real rb=1., 
		  const real sa=0.,
		  const real sb=1.,
		  const real ta=0.,
		  const real tb=1. );
  // set absolute bounds for a restriction mapping
  int setBounds(const real ra=0., 
		const real rb=1., 
		const real sa=0.,
		const real sb=1.,
		const real ta=0.,
		const real tb=1. );

  int getBounds(real & ra, real & rb, real & sa, real & sb, real & ta, real & tb ) const;
  

protected:
  void constructor(Mapping & map, const ReparameterizationTypes type);
  void constructorForMultipleReparams(ReparameterizationTransform & map );
  void setMappingProperties(Mapping *mapPointer);

  int initializeEquidistribution(const bool & useOriginalMapping = TRUE);

 protected:
  aString className;
  Mapping *reparameterize;                 // points to either orthographic or restriction
  MappingParameters localParams;           // needed so we can invert in a different coordinate system
  int coordinateType; 
  ReparameterizationTypes reparameterizationType;
  real arcLengthWeight, curvatureWeight;
  int numberOfEquidistributionSmooths;
  bool equidistributionInitialized;
  
  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((ReparameterizationTransform &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((ReparameterizationTransform &)x); }    
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new ReparameterizationTransform(*this, ct); }
  };


#endif
