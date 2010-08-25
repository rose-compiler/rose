#ifndef OGEN_H 
#define OGEN_H "ogen.h"

#include "Overture.h"
#include "PlotStuff.h"
#include "MappingInformation.h"

class Ogen
{
 public:
  enum MovingGridOption
  {
    useOptimalAlgorithm=0,
    minimizeOverlap=1,
    useFullAlgorithm
  };


  Ogen();
  Ogen(PlotStuff & ps);
  ~Ogen();
  
  int changeParameters( CompositeGrid & cg );

  // build a composite grid interactively from scratch
  int updateOverlap( CompositeGrid & cg, MappingInformation & mapInfo );

  // build a composite grid non-interactively using the component grids found
  // in cg. This function might be called if one or more grids have changed.
  int updateOverlap( CompositeGrid & cg );

  // build a composite grid when some grids have moved, tryng to use optimized algorithms.
  int updateOverlap(CompositeGrid & cg, 
		    CompositeGrid & cgNew, 
		    const LogicalArray & hasMoved, 
		    const MovingGridOption & option =useOptimalAlgorithm );

  // update refinement level(s) on a composite grid.
  int updateRefinement(CompositeGrid & cg, 
                       const int & refinementLevel=-1 );

  void turnOnHybridHoleCutting() {classifyHolesForHybrid = TRUE;}
  void turnOffHybridHoleCutting() {classifyHolesForHybrid = FALSE;}
  int buildACompositeGrid(CompositeGrid & cg, 
			  MappingInformation & mapInfo, 
			  const IntegerArray & mapList,
			  const int & numberOfMultigridLevels =1,
                          const bool useAnOldGrid=FALSE );


 protected:

  int buildBounds( CompositeGrid & cg );
  
  int buildCutout(CompositeGrid & cg, MappingInformation & cutMapInfo );

  bool canDiscretize( MappedGrid & g, const int iv[3] );
  
  int interpolateMixedBoundary(CompositeGrid & cg, int mixedBoundaryNumber );
  
  int interpolateMixedBoundary(CompositeGrid & cg, int mixedBoundaryNumber,
                               int side, int axis, int grid, MappedGrid & g, int grid2, MappedGrid & g2,
			       int offset[3], real rScale[3],real rOffset[3]);

  int checkInterpolationOnBoundaries(CompositeGrid & cg);

  int classifyPoints(CompositeGrid & cg,
                     realArray & invalidPoint, 
		     int & numberOfInvalidPoints,
		     const int & level,
		     CompositeGrid & cg0 );

  int classifyRedundantPoints( CompositeGrid& cg, const int & grid, 
                               const int & level, 
			       CompositeGrid & cg0  );

  int computeOverlap( CompositeGrid & cg, 
		      CompositeGrid & cgOld,
                      const int & level=0,
		      const bool & movingGrids =FALSE, 
		      const IntegerArray & hasMoved = Overture::nullIntArray() );

  int checkForOrphanPointsOnBoundaries(CompositeGrid & cg );

  int cutHoles(CompositeGrid & cg );
  // int cutHolesNew(CompositeGrid & cg );

  int countCrossingsWithRealBoundary(CompositeGrid & cg, 
                                     const realArray & x, 
				     IntegerArray & crossings );
  
  int findTrueBoundary(CompositeGrid & cg);

  int interpolateAll(CompositeGrid & cg, IntegerArray & numberOfInterpolationPoints, CompositeGrid & cg0);

  bool isNeededForDiscretization(MappedGrid& g, const int iv[3] );
  bool isOnInterpolationBoundary(MappedGrid& g, const int iv[3], const int & width=1 );
  bool isNeededForMultigridRestriction(CompositeGrid& c,
                                       const int & grid,
				       const int & l,
				       const int iv[3]);
  real computeInterpolationQuality(CompositeGrid & cg, const int & grid,
                                   const int & i1, const int & i2, const int & i3,
                                   real & qForward, real & qReverse );
  
  int markPointsNeededForInterpolation( CompositeGrid & cg, const int & grid, const int & lowerOrUpper=-1 );
  
  int markPointsReallyNeededForInterpolation( CompositeGrid & cg );

  int markPartiallyPeriodicBoundaries( CompositeGrid & cg,
                                       intArray *iInterp  );
  
  int improveQuality( CompositeGrid & cg, const int & grid, RealArray & removedPointBound );
  int updateCanInterpolate( CompositeGrid & cg, CompositeGrid & cg0, RealArray & removedPointBound );

  int plot(const aString & title,
           CompositeGrid & cg,
           const int & queryForChanges =TRUE );
  
  int projectToBoundary( CompositeGrid & cg,
                         const int & grid, 
			 const realArray & r,
			 const int iv[3], 
			 const int ivp[3], 
			 real rv[3] );

  int queryAPoint(CompositeGrid & cg);
  
  // int projectGhostPoints(CompositeGrid & cg);

  int removeExteriorPoints(CompositeGrid & cg, 
                           const bool boundariesHaveCutHoles= FALSE );
  
  int removeExteriorPointsNew(CompositeGrid & cg, 
                           const bool boundariesHaveCutHoles= FALSE );
  
  int unmarkBoundaryInterpolationPoints( CompositeGrid & cg, const int & grid );

  int unmarkInterpolationPoints( CompositeGrid & cg, const bool & unMarkAll=FALSE );

  int updateGeometry(CompositeGrid & cg,
                     CompositeGrid & cgOld,
		     const bool & movingGrids=FALSE, 
		     const IntegerArray & hasMoved = Overture::nullIntArray() );
  
 public:
  int debug;   // for turning on debug info and extra plotting
  int info;    // bit flag for turning on info messages
  bool useNewMovingUpdate;
  int defaultInterpolationIsImplicit;

 protected:
  // repeat some enumerators to simplify  
  enum
  {
    THEinverseMap = CompositeGrid::THEinverseMap,
    THEmask = CompositeGrid::THEmask,
    resetTheGrid=123456789,
    ISnonCuttingBoundaryPoint=MappedGrid::ISreservedBit1
  };

  PlotStuff *ps;
  PlotStuffParameters psp;
  FILE *logFile;    // log file for users
  FILE *checkFile;  // contains data on the grid that we can use to check when we change the code.
  
  real boundaryEps;  // We can still interpolate from a grid provided we are this close (in r)
  RealArray gridScale;  // gridScale(grid) = maximum length of the bounding box for a grid
  RealArray rBound;     // rBound(0:1,.) : bounds on r for valid interpolation
  real maximumAngleDifferenceForNormalsOnSharedBoundaries;
  
  IntegerArray geometryNeedsUpdating;  // true if the geometry needs to be updated after changes in parameters
  bool numberOfGridsHasChanged;  // true if we need to update stuff that depends on the number of grids
  bool checkForOneSided;
  IntegerArray isNew;           // this grid is in the list of new grids (for the incremental algorithm)
  
  int numberOfHolePoints;
  realArray holePoint;
  int numberOfOrphanPoints;
  realArray orphanPoint;
  
  int maskRatio[3];  // for multigrid: ratio of coarse to fine grid spacing for ray tracing
  
  int holeCuttingOption;   // 0=old way, 1=new way
  bool useAnOldGrid;
  
  int numberOfArrays;   // to check for memory leaks, track of how many arrays we have.
  
  // warnForSharedSides : TRUE if we have warned about possible shared sides not being marked properly
  IntegerArray warnForSharedSides;  // warnForSharedSides(grid,side+2*axis,grid2,side2+2*dir)

  // for mixed physical-interpolation boundaries
  int numberOfMixedBoundaries;
  IntegerArray mixedBoundary;
  RealArray mixedBoundaryValue;

  intArray *backupValues;  // holds info on backup values.
  IntegerArray backupValuesUsed;   // backupValuesUsed(grid) = TRUE is some backup values have been used
  
  MappingInformation cutMapInfo;  // put here temporarily. Holds curves that cut holes.
  bool useBoundaryAdjustment;
  bool improveQualityOfInterpolation;
  real qualityBound;
  bool minimizeTheOverlap;    // *** this should be in the CompositeGrid
  bool allowHangingInterpolation;
  bool allowBackupRules;
  bool classifyHolesForHybrid;

  char buff[200];

  real totalTime;   // total CPU time used to generate the grid
  real timeUpdateGeometry;
  real timeInterpolateBoundaries;
  real timeCutHoles;
  real timeFindTrueBoundary;
  real timeRemoveExteriorPoints;
  real timeImproperInterpolation;
  real timeProperInterpolation;
  real timeAllInterpolation;
  real timeRemoveRedundant;
  real timeImproveQuality;
  
  int adjustBoundary(CompositeGrid & cg,
                     const Integer&      k1,
		     const Integer&      k2,
		     const intArray& i1,
		     const realArray&    x);

  int adjustForNearbyBoundaries(CompositeGrid & cg,
                                IntegerArray & numberOfInterpolationPoints,
                                intArray *iInterp );
  
  bool canInterpolate(CompositeGrid & cg,
		      const Integer&      k10,
		      const Integer&      k20,
		      const RealArray&    r,
		      const LogicalArray& ok,
		      const LogicalArray& useBackupRules,
		      const Logical       checkForOneSided);

  int checkCrossings(CompositeGrid & cg,
                     const int & numToCheck, 
		     const IntegerArray & ia, 
		     intArray & mask,
		     realArray & x,
		     realArray & vertex,
		     IntegerArray & crossings,
		     const Range & Rx,
                     const int & usedPoint );
  
  int checkHoleCutting(CompositeGrid & cg);
  
  int computeInterpolationStencil(CompositeGrid & cg, 
				  const int & grid, 
				  const int & gridI, 
				  const real r[3], 
				  int stencil[3][2],
				  bool useOneSidedAtBoundaries = TRUE );

  int conformToCmpgrd( CompositeGrid & cg );

  int determineMinimalIndexRange( CompositeGrid & cg );

  int estimateSharedBoundaryTolerance(CompositeGrid & cg);

  int findBestGuess(CompositeGrid & cg, 
		    const int & grid, 
		    const int & numberToCheck, 
		    intArray & ia, 
		    realArray & x, 
		    realArray & r,
                    realArray & rI,
                    intArray & inverseGrid,
                    const realArray & center );

  int generateInterpolationArrays( CompositeGrid & cg, 
                                   const IntegerArray & numberOfInterpolationPoints,
				   intArray *iInterp );
  
  int initialize();

  int lastChanceInterpolation(CompositeGrid & cg,
                              CompositeGrid & cg0, 
			      const int & grid,
			      const int iv[3],
			      bool & ok,
                              intArray & interpolates,
			      int & numberOfInvalidPoints,
			      realArray & invalidPoint,
			      const int & printDiagnosticMessages = FALSE,
                              const bool & tryBackupRules = FALSE );

  int movingUpdate(CompositeGrid & cg, 
                   CompositeGrid & cgOld, 
		   const LogicalArray & hasMoved, 
		   const MovingGridOption & option =useOptimalAlgorithm );

  int resetGrid( CompositeGrid & cg );
  
  int setGridParameters(CompositeGrid & cg );

  int updateBoundaryAdjustment( CompositeGrid & cg, 
				const int & grid, 
				const int & grid2,
				intArray *iag,
				realArray *rg,
				realArray *xg,
				IntegerArray & sidesShare );

  int updateParameters(CompositeGrid & cg, const int level = -1);
  int updateMaskPeriodicity(MappedGrid & c, const int & i1, const int & i2, const int & i3 );
  int getNormal(const int & numberOfDimensions, const int & side, const int & axis, 
                const RealArray & xr, RealArray & normal);

  int markOffAxisRefinementMask( int numberOfDimensions, Range Ivr[3], Range Ivb[3], int rf[3], 
                         intArray & mask, const intArray & maskb );

  int setRefinementMaskFace(intArray & mask,
                            int side, int axis, 
			    int numberOfDimensions, int rf[3],
			    Range & I1r, Range & I2r, Range & I3r,
			    const intArray & mask00, 
			    const intArray & mask10,
			    const intArray & mask01,
			    const intArray & mask11);
  
  int checkRefinementInterpolation( CompositeGrid & cg );
  

};


#endif

