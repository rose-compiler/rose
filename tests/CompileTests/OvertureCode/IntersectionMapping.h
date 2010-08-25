#ifndef INTERSECTION_MAPPING_H
#define INTERSECTION_MAPPING_H 

#include "Mapping.h"
class GenericGraphicsInterface;

#include "GenericGraphicsInterface.h"

//-------------------------------------------------------------
//  Define a Mapping by the intersection of two other mappings
//-------------------------------------------------------------
class IntersectionMapping : public Mapping
{
private:
  aString className;

  Mapping *map1,*map2; // pointers to Mapping's that are intersected


public:

  Mapping *rCurve1, *rCurve2, *curve;  // intersection curve, three representations
  

  IntersectionMapping();

  IntersectionMapping(Mapping & map1, 
                      Mapping & map2 );

  // IntersectionMapping(Mapping & surface, const real & TFIDistance=0. );

  // Copy constructor is deep by default
  IntersectionMapping( const IntersectionMapping &, const CopyType copyType=DEEP );

  ~IntersectionMapping();

  IntersectionMapping & operator =( const IntersectionMapping & X0 );

  int intersect(Mapping & map1, 
                Mapping & map2,
                GenericGraphicsInterface *gi=NULL,
                GraphicsParameters & params=Overture::defaultGraphicsParameters());

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  virtual int project( realArray & x,    // project points onto the curve of intersection
                       int & iStart, 
		       int & iEnd  );  
  virtual int project( realArray & x, 
                       MappingProjectionParameters & mpParams ){ return Mapping::project(x,mpParams); }
  
  virtual int reparameterize(const real & arcLengthWeight=1.,       // reparameterize by arc-length and curvature
                             const real & curvatureWeight=.5 );
  
  // intersect two curves in 2D
  int intersectCurves(Mapping & map1, Mapping & map2, 
		      int & numberOfIntersectionPoints, 
		      realArray & r1, 
		      realArray & r2,
		      realArray & x );
  
  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return IntersectionMapping::className; }

  int update( MappingInformation & mapInfo ) ;

 protected:
  int determineIntersection(GenericGraphicsInterface *gi=NULL,
                            GraphicsParameters & params=Overture::defaultGraphicsParameters());

  int newtonIntersection(realArray & x, realArray & r1, realArray & r2, const realArray & n );

  BoundingBox* createBoundingBox( BoundingBox & child1, BoundingBox & child2 ) ;
  void destroyBoundingBox( BoundingBox & box );
  

  private:


  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((IntersectionMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((IntersectionMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new IntersectionMapping(*this, ct); }

};


#endif  
