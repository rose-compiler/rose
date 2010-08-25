#ifndef COMPOSITE_SURFACE_H
#define COMPOSITE_SURFACE_H

#include "Mapping.h"
#include "ListOfMappingRC.h"
  
class GenericGraphicsInterface;
class GraphicsParameters;
class MappingProjectionParameters;
class CompositeTopology;

//---------------------------------------------------------------
//  Define a Composite Surface consisting of a set of Mappings
//---------------------------------------------------------------
class CompositeSurface  : public Mapping
{
 public:

  CompositeSurface();

  // Copy constructor is deep by default
  CompositeSurface( const CompositeSurface &, const CopyType copyType=DEEP );

  ~CompositeSurface();

  CompositeSurface & operator =( const CompositeSurface & X0 );

  int numberOfSubSurfaces() const;
   
  // return the Mapping that represents a subSurface
  Mapping & operator []( const int & subSurfaceIndex );

  // add a surface to the composite surface
  int add( Mapping & surface,
           const int & surfaceID = -1 );
  // Is this surface visible?
  int isVisible(const int & surfaceNumber) const;

  // set the visibility
  int setIsVisible(const int & surfaceNumber, const bool & trueOrFalse=TRUE);

  // remove a surface from the composite surface
  int remove( const int & surfaceNumber );

  // set the colour of a subSurface.
  int setColour( const int & surfaceNumber, const aString & colour );
  aString getColour( const int & surfaceNumber ) const;

  CompositeTopology* getCompositeTopology() const {return compositeTopology;}

  // project points onto the surface
  int project( realArray & x, MappingProjectionParameters & mpParameters );

  // project points onto the surface, old interface:
  void project( intArray & subSurfaceIndex,
		realArray & x, 
		realArray & rProject, 
		realArray & xProject,
		realArray & xrProject,
                realArray & normal = Overture::nullRealDistributedArray(),
                const intArray & ignoreThisSubSurface  = Overture::nullIntegerDistributedArray() );

  // project points onto the surface
  void oldProject( intArray & subSurfaceIndex,
                   realArray & x, 
		   realArray & rProject, 
		   realArray & xProject,
		   realArray & xrProject,
		   realArray & normal = Overture::nullRealDistributedArray(),
		   const intArray & ignoreThisSubSurface  = Overture::nullIntegerDistributedArray() );
  
  virtual void map( const realArray & r, 
		    realArray & x, 
		    realArray & xr = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return CompositeSurface::className; }

  int update( MappingInformation & mapInfo ) ;

  int printStatistics(FILE *file=stdout);

// AP experimenting with display lists...
enum DisplayListProperty 
{
  boundary=0, gridLines, shadedSurface, numberOfDLProperties
};

IntegerArray dList; // int i; DisplayListContents j; dList(j,i) == display list number for surface i, feature j

 protected:
  void initialize();
  void determineTopology();
  int getNormals(const intArray & subSurfaceIndex, const realArray & xr, realArray & normal) const;
  void findNearbySurfaces( const int & s, 
                           realArray & r,
			   const bool & doubleCheck,
			   IntegerArray & consistent,
			   IntegerArray & inconsistent );
  int  findOutwardTangent( Mapping & map, const realArray & r,  const realArray & x, realArray & outwardTangent );
  void plotSubSurfaceNormals(GenericGraphicsInterface & gi, GraphicsParameters & params);
  
  IntegerArray visible;  // is a sub surface visible
  IntegerArray surfaceIdentifier;  // user defined identifier for the surface
  aString *surfaceColour;   // colour for the subSurface.

  enum TimingEnum
  {
    totalTime,
    timeToProject, 
    timeToProjectInvertMapping, 
    timeToProjectEvaluateMapping,
    numberOfTimings
  };
  real timing[numberOfTimings];


 private:

  aString className;
  int numberOfSurfaces;

  ListOfMappingRC surfaces;   // list of surfaces

  CompositeTopology *compositeTopology;
  IntegerArray signForNormal; // multiply normal by this (+1,-1) so that all normals are consistent
  bool topologyDetermined;

 private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((CompositeSurface &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((CompositeSurface &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new CompositeSurface(*this, ct); }

};


#endif  
