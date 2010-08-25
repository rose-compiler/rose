#ifndef DEPTH_MAPPING_H
#define DEPTH_MAPPING_H 

#include "Mapping.h"
  
//-------------------------------------------------------------
//  Define a 3D Mapping from 2D Mapping by extending in 
//  the z-direction by a variable amount
//-------------------------------------------------------------
class DepthMapping : public Mapping
{

public:

  enum DepthOption
  {
    constantDepth,
    quadraticDepth,
    depthFunction
  };

  DepthMapping();
  // Copy constructor is deep by default
  DepthMapping( const DepthMapping &, const CopyType copyType=DEEP );

  ~DepthMapping();

  DepthMapping & operator =( const DepthMapping & X0 );

  int setDepthFunction( Mapping & depth );
  int setDepthFunctionParameters( real a0, real b0, real a1, real b1 );
  int setSurface( Mapping & surface );
  int setQuadraticParameters(const real & a00,
                             const real & a10, 
			     const real & a01, 
			     const real & a20, 
			     const real & a11, 
			     const real & a02);

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return DepthMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:
  aString className;
  Mapping *surface;    // this is a 2D grid for the surface
  Mapping *depth;      // This mapping may define a depth, z=depth(x,y)

  DepthOption depthOption;
  
  real zSurface;       // z level for the surface
  real zDepth;         // depth (if constant)
  real a00,a10,a01,a20,a11,a02;  // quadratic depth parameters
  real depthPar[4];  // scale factors from x to r for the depthFunction
  
  int initialize();

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((DepthMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((DepthMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new DepthMapping(*this, ct); }

};


#endif  
