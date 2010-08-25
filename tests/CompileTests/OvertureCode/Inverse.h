#ifndef INVERSE_H
#define INVERSE_H "Inverse.h"

#include "Mapping.h"
#include "BoundingBox.h"  // define BoundingBox and BoundingBoxStack

class Mapping;
class IntersectionMapping;
class GenericDataBase;

//===========================================================================
//  Class to define an Approximate Global Inverse
//
//===========================================================================
class ApproximateGlobalInverse
{
 friend class Mapping;
 friend class IntersectionMapping;
 public:
  static const real bogus;   // Bogus value to indicate no convergence

 protected:

  Mapping *map;
  int domainDimension;
  int rangeDimension;
  int uninitialized;
  bool useRobustApproximateInverse;
  
  int gridDefined;  
  realArray & grid;           // holds grid points, reference to map->grid
  IntegerArray dimension;       // holds dimensions of grid
  IntegerArray indexRange;      // holds index range for edges of the grid, may depend on singularities

  void constructGrid();
  int base,bound;
  int base0,bound0;

  RealArray boundingBox;   // grid is contained in this box

  BoundingBox boundingBoxTree[2][3];  // root of tree of boxes for each side
  BoundingBox box,box1,box2;
  //  BoundingBoxStack boxStack;

  real boundingBoxExtensionFactor;  // relative amount to increase the bounding box each direction.
  real stencilWalkBoundingBoxExtensionFactor;  // stencil walk need to converge on a larger region
  bool findBestGuess;   // always find the best guess (ignore bounding boxes);

  RealArray xOrigin;
  RealArray xTangent;

  Index Axes;
  Index xAxes;

  // void setGrid( const realArray & grid, const IntegerArray & gridIndexRange  );

  void getPeriodicImages( const realArray & x, realArray & xI, int & nI, 
                          const int & periodicityOfSpace, const RealArray & periodicityVector );


 public:  // ** for now ***
  
  void intersectLine( const realArray & x, int & nI, realArray & xI, 
		  const RealArray & vector, const RealArray & xOrigin, const RealArray & xTangent );
  void intersectPlane( const realArray & x, int & nI, realArray & xI, 
		  const RealArray & vector, const RealArray & xOrigin, const RealArray & xTangent );
  void intersectCube( const realArray & x, int & nI, realArray & xI, 
		  const RealArray & vector, const RealArray & xOrigin, const RealArray & xTangent );

  void initializeBoundingBoxTrees();
  void binarySearchOverBoundary( real x[3], real & minimumDistance, int iv[3], int side=-1, int axis=-1 );
/* ---
  void robustBinarySearchOverBoundary( real x[3], 
                                       real & minimumDistance, 
				       int iv[3],
				       int side,
				       int axis  );
--- */
  int insideGrid( int side, int axis, real x[], int iv[], real & dot );

  int distanceToCell( real x[], int iv[], real & signedDistance );
  
  void findNearestGridPoint( const int base, const int bound, realArray & x, realArray & r );
  int findNearestCell(real x[3], int iv[3], real & minimumDistance );
  
  void initializeStencilWalk();
  void countCrossingsWithPolygon(const realArray & x, 
                                 IntegerArray & crossings,
                                 const int & side=Start, 
                                 const int & axis=axis1,
                                 realArray & xCross = Overture::nullRealDistributedArray(),
				 const IntegerArray & mask = Overture::nullIntArray(),
                                 const  unsigned int & maskBit = UINT_MAX,   // (UINT_MAX : all bits on)
				 const int & maskRatio1 =1 ,
				 const int & maskRatio2 =1 ,
				 const int & maskRatio3 =1 );
  
 public:
  ApproximateGlobalInverse( Mapping & map );
  virtual ~ApproximateGlobalInverse();
  virtual void inverse( const realArray & x, realArray & r, realArray & rx, 
                       MappingWorkSpace & workSpace, MappingParameters & params );

  void initialize();     // initialize if not already done so
  void reinitialize();   // this will force a re-initialize the inverse
  const realArray & getGrid() const;  // return the grid used for the inverse
  const RealArray & getBoundingBox() const;
  
  real         getParameter( const MappingParameters::realParameter & param ) const;
  int          getParameter( const MappingParameters::intParameter & param ) const;
  virtual void setParameter( const MappingParameters::realParameter & param, const real & value );
  virtual void setParameter( const MappingParameters::intParameter & param, const int & value );
  virtual void useRobustInverse(const bool trueOrFalse=TRUE );
  virtual bool usingRobustInverse() const;

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

 public:
  // these are used for timings and statistics
  static real timeForApproximateInverse,
    timeForFindNearestGridPoint,
    timeForBinarySearchOverBoundary,
    timeForBinarySearchOnLeaves;
  
  static int numberOfStencilWalks,
             numberOfStencilSearches,
             numberOfBinarySearches,
             numberOfBoxesChecked,
             numberOfBoundingBoxes;

  static void printStatistics();

  // these next arrays are for the stencil walk
  static int numberOfStencilDir2D[9];  // (3,3);
  static int stencilDir2D1[8*3*3];    // (8,3,3)
  static int stencilDir2D2[8*3*3];
  static int stencilDir2D3[8*3*3];
  static int numberOfStencilDir3D[27];  // (3,3,3);
  static int stencilDir3D1[27*3*3*3];    // (27,3,3,3)
  static int stencilDir3D2[27*3*3*3];
  static int stencilDir3D3[27*3*3*3];

};


//===========================================================================
//  Class to define an Exact Local Inverse
//
//===========================================================================
const int maximumNumberOfRecursionLevels=5;

class ExactLocalInverse
{
 private:
  Mapping *map;
  int domainDimension;
  int rangeDimension;
  RealArray periodVector;
  int base,bound;
  Index Axes;
  Index xAxes;
  int uninitialized;

  real nonConvergenceValue;  // value given to inverse when there is no convergence
  real newtonToleranceFactor;  // convergence tolerance is this times the machine epsilon
  real newtonDivergenceValue;  // newton is deemed to have diverged if the r value is this much outside [0,1]

  // Work Arrays for Newton:
  // realArray y,yr,r2,yr2;   // these arrays cannot be static if the inverse is called recursively
  // bool workArraysTooSmall;

  void initialize();
  inline void periodicShift( realArray & r, const Index & I );
  void underdeterminedLS(const realArray & xt, 
                         const realArray & tx,   // *** should not be const -- do for IBM compiler
                         const realArray & dy,
          		 const realArray & dr,   // *** should not be const -- do for IBM compiler
                         real & det );

  inline void invert(realArray & yr, realArray & dy, realArray & det, 
                     realArray & ry, realArray & dr, intArray & status);
  inline void invertL2(realArray & yr, realArray & dy, realArray & det, realArray & yr2, realArray & yrr, 
                       realArray & ry, realArray & dr, intArray & status );
  void minimizeByBisection(realArray & r, realArray & x, realArray & dr, intArray & status, real & eps );

 public:
  ExactLocalInverse( Mapping & map );
  virtual ~ExactLocalInverse();
  virtual void inverse( const realArray & x1, realArray & r1, realArray & rx1, 
                       MappingWorkSpace & workSpace, const int computeGlobalInverse=FALSE );

  void reinitialize();

  real         getParameter( const MappingParameters::realParameter & param ) const;
  virtual void setParameter( const MappingParameters::realParameter & param, const real & value );

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

 public:
  static real timeForExactInverse;  
  static int numberOfNewtonInversions,
             numberOfNewtonSteps;
  
 protected:
  bool mappingHasACoordinateSingularity;

  int compressConvergedPoints(Index & I,
			      realArray & x, 
			      realArray & r, 
			      realArray & ry, 
			      realArray & det, 
			      intArray & status,
			      const realArray & x1, 
			      realArray & r1, 
			      realArray & rx1, 
			      MappingWorkSpace & workSpace,
			      const int computeGlobalInverse );
  
};




#endif  // "Inverse.h"
