#ifndef MAPPING_H
#define MAPPING_H "Mapping.h"

//-----------------------------------------------------------------
//  Class Mapping : Class to define a mapping for overlapping grids
//
//  Define a mapping from the parameter space (domain) into
//  physical space (range)
//
//-----------------------------------------------------------------

#include <assert.h>
#include "A++.h"
#include "OvertureTypes.h"     // define real to be float or double
#include "wdhdefs.h"           // some useful defines and constants
#include "mathutil.h"          // define max, min,  etc
#include "Bound.h"             // defines the Bound type
#include "MappingP.h"          // defines MappingParameters

#include "OvertureInit.h"

#include "Inverse.h"           // defines global and local inverses
#include <strstream.h>         // for streams to strings
#include "aString.H"            // Livermore aString Library
#include "ReferenceCounting.h"
#include "GenericDataBase.h"

class MappingRC;             // forward declaration
class MappingInformation;    // forward declaration
class MappingProjectionParameters;
class BoundingBox;
class GenericGraphicsInterface;

// **** here are global variables ****
// extern MappingParameters Overture::nullMappingParameters();


class Mapping : public ReferenceCounting
{

 public:

  // Here are enumerators for the possible spaces for the domain and range
  enum mappingSpace
  {
    parameterSpace,    // bounds are [0,1]
    cartesianSpace     // default (-infinity,infinity)
  };

  // Here are enumerators for the coordinate systems that we can use (also in MappingP.h)
  // Derivatives of mappings can be returned in these different systems
  enum coordinateSystem
  {
    cartesian,                 //  x,y,z
    spherical,                 //  phi/pi, theta/2pi, r
    cylindrical,               //  theta/2pi, z, r 
    polar,                     //  r, theta/tpi, z
    toroidal,                  //  theta1/tpi, theta2/tpi, theta3/tpi
    numberOfCoordinateSystems  //  keeps a count of the number of enums in this list
  };

  // Here are enumerators for the types of coordinate singularity a mapping may have
  enum coordinateSingularity
  {
    noCoordinateSingularity,   //  no coordinate singularity
    polarSingularity           //  grid lines go to a point along the side
  };


  // Here are enumerators for the types of Mapping coordinate systems, these are
  // used to optimize the computation of difference approximations to functions
  // defined on grids derived from this mapping
  enum mappingCoordinateSystem
  {
    rectangular,               // rectangular mapping
    conformal,                 // conformal              : metric tensor is diagonal and ...
    orthogonal,                // orthogonal mapping     : metric tensor is diagonal
    general                    // general transformation : no special properties
  };

  // Here are the enumerators for isPeriodic
  enum periodicType
  {
    notPeriodic,
    derivativePeriodic,    // Derivative is periodic but not the function
    functionPeriodic       // Function is periodic
  };
      
  enum topologyEnum
  {
    topologyIsNotPeriodic=0,
    topologyIsPeriodic,
    topologyIsPartiallyPeriodic    // this is a "C" grid.
  };

  // Here are enumerators for the items that we save character names for:
  enum mappingItemName
  {
    mappingName,      // mapping name
    domainName,       // domain name
    rangeName,
    domainAxis1Name, // names for coordinate axes in domain
    domainAxis2Name, 
    domainAxis3Name, 
    rangeAxis1Name,  // names for coordinate axes in range
    rangeAxis2Name, 
    rangeAxis3Name,
    numberOfMappingItemNames    //  keeps a count of the number of enums in this list
  };

  enum basicInverseOptions  // options for basicInverse
  {
    canDoNothing,
    canDetermineOutside,
    canInvert
  };

  Mapping( int domainDimension=3, 
           int rangeDimension=3, 
           mappingSpace domainSpace=parameterSpace,
           mappingSpace rangeSpace=cartesianSpace,
           coordinateSystem domainCoordinateSystem=cartesian,
           coordinateSystem rangeCoordinateSystem=cartesian
            ); 

  // Copy constructor is deep by default
  Mapping( const Mapping &, const CopyType copyType=DEEP );

  //  assignment with = is a deep copy
  Mapping & operator =( const Mapping & X );
  
  virtual ~Mapping();
  
  void reference( const Mapping & map );
  void breakReference();

  // This function is used to create a new member of the Class provided the
  // mappingClassName is equal to the name of the class
  virtual Mapping *make( const aString & mappingClassName );

  // Map the domain r to the range x
  virtual void map( const realArray & r, realArray & x, realArray &xr = Overture::nullRealDistributedArray(),
                    MappingParameters & params =Overture::nullMappingParameters());

  // Map the range x back to the domain r
  virtual void inverseMap( const realArray & x, realArray & r, realArray & rx =Overture::nullRealDistributedArray(),
			  MappingParameters & params =Overture::nullMappingParameters());

  // If you know the inverse of your mapping supply this next function, 
  //      and set basicInverseOption=canInvert
  // If you don't know the inverse but know how to determine if a point is not in the
  // range (better than a bounding box) then set supply this function,
  //      and  set basicInverseOption=canDetermineOutside
  virtual void basicInverse(const realArray & x, 
			    realArray & r,
			    realArray & rx =Overture::nullRealDistributedArray(),
			    MappingParameters & params =Overture::nullMappingParameters());

  // Here are versions of map and inverseMap needed by some compilers (IBM:xlC) that don't like passing
  // views of arrays to non-const references, as in mapping.mapC(r(I),x(I),xr(I))
  virtual void mapC( const realArray & r, const realArray & x, const realArray &xr = Overture::nullRealDistributedArray(),
                    MappingParameters & params =Overture::nullMappingParameters());
  virtual void inverseMapC( const realArray & x, const realArray & r, const realArray & rx =Overture::nullRealDistributedArray(),
			  MappingParameters & params =Overture::nullMappingParameters());

  // map a grid of points: r(0:n1,1), or r(0:n1,0:n2,2) or r((0:n1,0:n2,0:n3,3) for 1, 2 or 3d
  virtual void mapGrid(const realArray & r, 
                       realArray & x, 
		       realArray & xr =Overture::nullRealDistributedArray(),
		       MappingParameters & params=Overture::nullMappingParameters() );

  // determine if one mapping (or face of) intersects another mapping (or face of)
  virtual int intersects(Mapping & map2, 
                         const int & side1=-1, 
			 const int & axis1=-1,
			 const int & side2=-1, 
			 const int & axis2=-1,
			 const real & tol=0. ) const;

  // inverse map a grid of points
  virtual void inverseMapGrid(const realArray & x, 
			      realArray & r, 
			      realArray & rx =Overture::nullRealDistributedArray(),
			      MappingParameters & params=Overture::nullMappingParameters() );
  

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  virtual int update( MappingInformation & mapInfo );  // update mapping, change parameters interactively

  virtual bool updateWithCommand( MappingInformation &mapInfo, const aString &command ); // update using one command, possible interactively, return true if the command was understood

  virtual int interactiveUpdate( GenericGraphicsInterface & gi );  // calls above update

  // project points onto the Mapping, usually used when for curves or surfaces.
  virtual int project( realArray & x, MappingProjectionParameters & mpParams );

  virtual void display( const aString & label=blankString) const;

  int checkMapping(); // Check the mapping - check derivatives and inverse, return 0 if ok

  void reinitialize();   // re-initialize a mapping that has changed (this will re-initialize the inverse)
  
  int determineResolution(int numberOfGridPoints[], 
			  bool collapsedEdge[2][3],
			  real averageArclength[],
			  real elementDensityTolerance=.05 );
  

  void secondOrderDerivative(const Index & I,      // compute second derivatives by finite differences.
                             const realArray & r, 
			     realArray & xrr, 
			     const int axis,
			     const int & rAxis );
  
  static real epsilon();  // here is the epsilon used by the Mappings and related classes.

 //----------------get functions----------------------------- 
  basicInverseOptions      getBasicInverseOption() const;
  int                      getBoundaryCondition( const int side, const int axis ) const;
  // return the bounding box from the approximate local inverse
  RealArray                getBoundingBox( const int & side=-1, const int & axis=-1 ) const;
  const BoundingBox &      getBoundingBoxTree( const int & side, const int & axis ) const;
  virtual aString           getClassName() const;
  int                      getCoordinateEvaluationType( const coordinateSystem type ) const;
  Bound                    getDomainBound( const int side, const int axis ) const;
  int                      getDomainDimension() const;
  coordinateSystem         getDomainCoordinateSystem() const;
  Bound                    getDomainCoordinateSystemBound( const int side, const int axis ) const;
  mappingSpace             getDomainSpace() const;
  int                      getGridDimensions( const int axis ) const;
  virtual const realArray& getGrid(MappingParameters & params=Overture::nullMappingParameters());
  int                      getID() const;
  int                      getInvertible() const;
  periodicType             getIsPeriodic( const int axis ) const;
  mappingCoordinateSystem  getMappingCoordinateSystem() const;
  aString                  getName( const mappingItemName item ) const;
  real                     getParameter( const MappingParameters::realParameter & param ) const;
  int                      getParameter( const MappingParameters::intParameter & param ) const;
  real                     getPeriodVector( const int axis, const int direction ) const;
  Bound                    getRangeBound( const int side, const int axis ) const;
  coordinateSystem         getRangeCoordinateSystem() const;
  Bound                    getRangeCoordinateSystemBound( const int side, const int axis ) const;
  int                      getRangeDimension() const;
  mappingSpace             getRangeSpace() const;
  int                      getShare( const int side, const int axis ) const;
  real                     getSignForJacobian() const;
  topologyEnum             getTopology( const int side, const int axis ) const;
  coordinateSingularity    getTypeOfCoordinateSingularity( const int side, const int axis ) const;

  int hasACoordinateSingularity() const;

  intArray & topologyMask();

  //--------------set functions-------------------------------
  virtual void setBasicInverseOption( const basicInverseOptions option );
  virtual void setBoundaryCondition( const int side, const int axis, const int bc );
  virtual void setCoordinateEvaluationType( const coordinateSystem type, const int trueOrFalse );
  virtual void setTypeOfCoordinateSingularity( const int side, const int axis, const coordinateSingularity );
  virtual void setDomainBound( const int side, const int axis, const Bound domainBound );
  virtual void setDomainCoordinateSystem( const coordinateSystem domainCoordinateSystem );
  virtual void setDomainCoordinateSystemBound( const int side, const int axis,
                                       const Bound domainCoordinateSystemBound );
  virtual void setDomainDimension( const int domainDimension );
  virtual void setDomainSpace( const mappingSpace domainSpace );
  virtual void setGridDimensions( const int axis, const int dim );
  virtual void setInvertible( const int invertible );
  void setID();
  virtual void setIsPeriodic( const int axis, const periodicType isPeriodic );
  virtual void setMappingCoordinateSystem( const mappingCoordinateSystem mappingCoordinateSystem0 );
  virtual void setName( const mappingItemName item, const aString & name );
  virtual void setParameter( const MappingParameters::realParameter & param, const real & value );
  virtual void setParameter( const MappingParameters::intParameter & param, const int & value );
  virtual void setPeriodVector( const int axis, const int direction, 
				const real periodVectorComponent );
  virtual void setRangeBound( const int side, const int axis, const Bound rangeBound );
  virtual void setRangeCoordinateSystem( const coordinateSystem rangeCoordinateSystem );
  virtual void setRangeCoordinateSystemBound( const int side, const int axis,
                                      const Bound rangeCoordinateSystemBound );
  virtual void setRangeDimension( const int rangeDimension );
  virtual void setRangeSpace( const mappingSpace rangeSpace );
  virtual void setShare( const int side, const int axis, const int share );
  void setSignForJacobian( const real signForJac );
  virtual void setTopology( const int side, const int axis, const topologyEnum topo );
  
  virtual void useRobustInverse(const bool trueOrFalse=TRUE );

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  virtual int outside( const realArray & x );  // is x outside the grid? (false means NO or
                                               // don't know)

  void periodicShift( realArray & r, const Index & I ); // useful utility routine

  Index getIndex(const realArray & r, realArray & x, const realArray &xr,
                 int & base, int & bound, int & computeMap, int & computeMapDerivative );

  int computeMap,computeMapDerivative,base,bound;  // used by routines calling getIndex

// #ifdef USE_PPP
//   // Map the domain r to the range x
//   virtual void map( const RealArray & r, RealArray & x, RealArray & xr = Overture::nullRealArray(),
//                     MappingParameters & params =Overture::nullMappingParameters());

// #endif

/* ----
  // here are versions that take distributed arrays, they have a different name so that
  // derived classes don't have to define them when they redefine the map, inverseMap, ... etc.
  virtual void mapD( const realArray & r, realArray & x, realArray &xr = Overture::nullRealDistributedArray(),
                    MappingParameters & params =Overture::nullMappingParameters());
  virtual void inverseMapD( const realArray & x, realArray & r, realArray & rx =Overture::nullRealDistributedArray(),
			  MappingParameters & params =Overture::nullMappingParameters());
  virtual void basicInverseD(const realArray & x, 
			    realArray & r,
			    realArray & rx =Overture::nullRealDistributedArray(),
			    MappingParameters & params =Overture::nullMappingParameters());
  virtual void mapCD( const realArray & r, const realArray & x, const realArray &xr = Overture::nullRealDistributedArray(),
                    MappingParameters & params =Overture::nullMappingParameters());
  virtual void inverseMapCD( const realArray & x, const realArray & r, const realArray & rx =Overture::nullRealDistributedArray(),
			  MappingParameters & params =Overture::nullMappingParameters());
  virtual void mapGridD(const realArray & r, 
                       realArray & x, 
		       realArray & xr =Overture::nullRealDistributedArray(),
		       MappingParameters & params=Overture::nullMappingParameters() );

  virtual void inverseMapGridD(const realArray & x, 
			      realArray & r, 
			      realArray & rx =Overture::nullRealDistributedArray(),
			      MappingParameters & params=Overture::nullMappingParameters() );

  virtual int projectD( realArray & x, MappingProjectionParameters & mpParams );
----- */

 protected:

  int dataBaseID;              // unique identifier for this Mapping when saved in a database file.
  
  aString    className;         // Name of the Class, make private for gnu compiler
  int       domainDimension;   // Dimension of the parameter space (r)
  int       rangeDimension;    // Dimension of map in physical space (x)
  // IntegerArray  bc;                // Boundary condition type
  // IntegerArray  share;             // code to indicate if boundaries are shared
  // IntegerArray  isPeriodic;        // Is the mapping periodic 0=no, 1=derivative, 2=function
  // RealArray periodVector;      // Period vector for isPeriodic=1, periodVector(.,axis) is
  //                             // the vector in the axis=axis1 or axis=axis2.. direction
  int bc[2][3];
  int share[2][3];
  periodicType isPeriodic[3];
  real periodVector[3][3];
    
  int       invertible;        // Is the mapping invertible;
  basicInverseOptions basicInverseOption; // what can the  basicInverse function do?


  // IntegerArray  gridDimensions;    // number of points in each direction to use for plotting etc.
  int gridDimensions[3];
  int periodicityOfSpace; // =0,1,2,3 : number of directions in which
                          // space is periodic
//  IntegerArray typeOfCoordinateSingularity;
//  IntegerArray coordinateEvaluationType;  // which coordinate systems are implemented for the mapping 
  coordinateSingularity typeOfCoordinateSingularity[2][3];
  int coordinateEvaluationType[numberOfCoordinateSystems];
  
  
  realArray grid;        // If this exists it can be used for plotting the mapping or the inverse
  int mappingHasChanged();  // call this function if the mapping has changed
  bool gridIsValid() const;     // true if grid is up to date (remakeGrid==false)
  void setGridIsValid();     // set remakeGrid=false

  mappingSpace domainSpace;    // space of the domain
  mappingSpace rangeSpace;     // space of the range
  coordinateSystem domainCoordinateSystem ;  // domain coordinate system
  coordinateSystem rangeCoordinateSystem ;   // range coordinate system
  mappingCoordinateSystem mappingCoordinateSystem0;  // for optimizing derivatives

  Bound domainBound[3][2];                   // bounds on the domain
  Bound rangeBound[3][2];                    // bounds on the range
  Bound domainCoordinateSystemBound[3][2];
  Bound rangeCoordinateSystemBound[3][2];

  void setDefaultMappingBounds( const mappingSpace ms, Bound mappingBound[3][2] );

  void setDefaultCoordinateSystemBounds( const coordinateSystem cs, Bound csBound[3][2] );

  topologyEnum topology[3][2];
  intArray *topologyMaskPointer;

  real signForJacobian;

 private:
  bool remakeGrid;       // set by mappingHasChanged

  aString namestr[numberOfMappingItemNames];   // here is where we save the names of items

  int validSide( const int side ) const;
  int validAxis( const int axis ) const;
  void mappingError( const aString & subName,  const int side, const int axis ) const;

 // protected:  ** should be protected, make public for "space" test program
 public:
  static int useInitialGuessForInverse;  
  // MappingWorkSpace workSpace;  // work space for inverse
  ApproximateGlobalInverse *approximateGlobalInverse;
  ExactLocalInverse *exactLocalInverse;

 //----------------makeMapping-------------------------------------------------------------
 public:
  class Item  // structure to store the pointers to the mappings in the LinkedList
  {
   public:
    Mapping* val;    
    Item* next;
    Item(){};
    Item( Mapping* value ){ val=value; next=NULL; }
    ~Item(){}
  };
  
  // Linked List of Mappings for makeMapping
  class LinkedList   
  {
   public:
    Item *start;
    Item *end;
    LinkedList();
    ~LinkedList();
    void add( Mapping *val );
    int remove( Mapping *val );
  };
      
  static Mapping* makeMapping( const aString & mappingClassName );


  static Mapping::LinkedList & staticMapList();

 public:
  static int debug;     // variable used for debugging
  static const real bogus;   // Bogus value to indicate no convergence

  friend class MappingRC;
  friend class ApproximateGlobalInverse;
  
  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((Mapping &)x); }
    virtual void reference( const ReferenceCounting& x)
      { reference( (Mapping &) x); }
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new Mapping(*this, ct); }
};

// extern Mapping::LinkedList Mapping::staticMapList();  // list of Mappings for makeMapping  ** remove this **


#endif // MAPPING_H

