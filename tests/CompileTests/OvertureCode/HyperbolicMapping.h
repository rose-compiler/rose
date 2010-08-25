#ifndef HYPERBOLIC_MAPPING_H
#define HYPERBOLIC_MAPPING_H 

#include "Mapping.h"
#include "GenericGraphicsInterface.h"
#include "DataPointMapping.h"
#include "MappingProjectionParameters.h"

class TridiagonalSolver;
class DialogData;

//----------------------------------------------------------------------
//  Define a Mapping by Hyperbolic grid generation.
//----------------------------------------------------------------------
class HyperbolicMapping : public Mapping
{
public:

  enum HyperbolicParameter
  {
    numberOfRegionsInTheNormalDirection=0,
    stretchingInTheNormalDirection,
    linesInTheNormalDirection,  // typo corrected 2000-10-04 **pf
    boundaryConditions,
    dissipation,
    distanceToMarch, 
    spacing, 
    volumeParameters, 
    barthImplicitness,
    axisParameters,
    growInTheReverseDirection,
    growInBothDirections,
    projectGhostBoundaries
  };

  enum Direction
  {
    bothDirections=0,
    forwardDirection,
    reverseDirection
  };
  

  enum SpacingType
  {
    constantSpacing,
    geometricSpacing,
    inverseHyperbolicSpacing,
    oneDimensionalMappingSpacing,
    userDefinedSpacing    
  };

  enum BoundaryCondition
  {
    freeFloating=1, //  IBCJA  = -1  float X, Y and Z - zero order extrapolation (free floating)
    outwardSplay,   //         < -1  outward-splaying free floating boundary condition which bends
    fixXfloatYZ,
    fixYfloatXZ,
    fixZfloatXY,
    floatXfixYZ,
    floatYfixXZ,
    floatZfixXY,
    floatCollapsed,
    periodic,
    xSymmetryPlane,
    ySymmetryPlane,
    zSymmetryPlane,
    singularAxis,
    matchToMapping,
    matchToPlane,
    trailingEdge,
    matchToABoundaryCurve,
    numberOfBoundaryConditions
  };

  static aString boundaryConditionName[numberOfBoundaryConditions];

  enum InitialCurveEnum
  {
    initialCurveFromEdges,
    initialCurveFromCoordinateLine0,
    initialCurveFromCoordinateLine1,
    initialCurveFromCurveOnSurface,
    initialCurveFromBoundaryCurves
  };

  enum PickingOptionEnum
  {
    pickToChooseInitialCurve,
    pickOff
  } pickingOption;

  HyperbolicMapping();

  // Copy constructor is deep by default
  HyperbolicMapping( const HyperbolicMapping &, const CopyType copyType=DEEP );
  HyperbolicMapping(Mapping & surface_);
  // for a surface grid:
  HyperbolicMapping(Mapping & surface_, Mapping & startingCurve);
  
  ~HyperbolicMapping();

  HyperbolicMapping & operator =( const HyperbolicMapping & X0 );

  virtual void display( const aString & label=blankString) const;
  void  estimateMarchingParameters( real & estimatedDistanceToMarch, int & estimatedLinesToMarch );
  int getBoundaryCurves( int & numberOfBoundaryCurves_, Mapping **&boundaryCurves_ );
  int setBoundaryCurves( const int & numberOfBoundaryCurves_, Mapping **boundaryCurves_ );
  int deleteBoundaryCurves();
  
  // supply a mapping to match a boundary condition to.
  int setBoundaryConditionMapping(const int & side, 
				  const int & axis,
				  Mapping & map,
				  const int & mapSide=-1, 
				  const int & mapAxis=-1);
  
  // supply a curve/surface 
  int setSurface(Mapping & surface, bool isSurfaceGrid=true );
  void setIsSurfaceGrid( bool trueOrFalse );
  
  // supply a starting curve for a surface grid
  int setStartingCurve(Mapping & startingCurve );

  int setParameters(const HyperbolicParameter & par, 
		    const IntegerArray & ipar= Overture::nullIntArray(), 
		    const RealArray & rpar = Overture::nullRealArray(),
                    const Direction & direction = bothDirections );

  // save the reference surface and starting curve when 'put' is called.
  int saveReferenceSurfaceWhenPut(bool trueOrFalse = TRUE);

  int generateOld();  // uses hypegen
  int generate(const int & numberOfAdditionalSteps = 0 );
  int generateNew(const int & numberOfAdditionalSteps = 0 );  // same as above.
  
  virtual void useRobustInverse(const bool trueOrFalse=TRUE );

  virtual void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
		    MappingParameters & params =Overture::nullMappingParameters() );

  virtual void basicInverse(const realArray & x, 
			    realArray & r,
			    realArray & rx =Overture::nullRealDistributedArray(),
			    MappingParameters & params =Overture::nullMappingParameters());


  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping *make( const aString & mappingClassName );
  aString getClassName() const { return HyperbolicMapping::className; }

  int update( MappingInformation & mapInfo );
  int update( MappingInformation & mapInfo,
	      const aString & command,
	      DialogData *interface  =NULL  ) ;

 protected:

  int buildMarchingParametersDialog(DialogData & marchingParametersDialog, aString bcChoices[]);
  int assignMarchingParametersDialog(DialogData & marchingParametersDialog, aString bcChoices[] );
  bool updateMarchingParameters(aString & answer, DialogData & marchingParametersDialog, aString bcChoices[],
                                MappingInformation & mapInfo );

  int buildSurfaceGridParametersDialog(DialogData & surfaceGridParametersDialog);
  bool updateSurfaceGridParameters(aString & answer, DialogData & surfaceGridParametersDialog,
                                   MappingInformation & mapInfo,
                                   GraphicsParameters & referenceSurfaceParameters );
  // old way:
  bool updateOld(aString & answer,
                 MappingInformation & mapInfo,
		 GraphicsParameters & referenceSurfaceParameters );

  int initialize();
  int setup();
  int updateForInitialCurve();
  
  int initializeHyperbolicGridParameters();
  int hypgen(GenericGraphicsInterface & gi, GraphicsParameters & parameters);
  int smooth(GenericGraphicsInterface & gi, GraphicsParameters & parameters);

  int getNormalAndSurfaceArea(realArray & x, 
			      const int & i3, 
			      realArray & normal, 
			      realArray & s,
			      realArray & xr, 
			      realArray & xrr,
			      const real & dSign,
			      realArray & normXr,
			      realArray & normXs,
			      realArray & ss  );

  int getCurvatureDependentSpeed(realArray & ds, 
                                 realArray & kappa,
				 const realArray & xrr, 
				 const realArray & normal, 
				 const realArray & normXr, 
				 const realArray & normXs);
  
  int getNormalGridSpacing(const int & i3Delta, realArray & ds, const int & growthDirection );
  int adjustDistanceToMarch(const int & numberOfAdditionalSteps, const int & growthDirection );
  int applyBoundaryConditions( realArray & x, 
                               const int & i3p, 
			       const int & i3, 
			       const int & marchingDirection,
			       realArray & normal,
			       realArray & xr );
  int jacobiSmooth( realArray & ss, const int & numberOfSmooths );
  int formCMatrix(realArray & xr, 
		  realArray & xt,
                  const int & i3Mod2,
		  realArray & normal, 
		  realArray & normXr,
		  const int & direction );
  
  int computeNonlinearDiffussionCoefficient(const realArray & normXr, 
					    const realArray & normXs, 
					    const realArray & normXt, 
					    const int & direction );
  
  int formBlockTridiagonalSystem(const int & direction);

  int implicitSolve(realArray & xTri, 
		    const int & i3Mod2,
		    realArray & xr,
		    realArray & xt,
		    realArray & normal,
		    realArray & normXr,
		    realArray & normXs,
		    realArray & normXt,
		    TridiagonalSolver & tri);
  

  int createCurveFromASurface(GenericGraphicsInterface & gi, Mapping & surface, Mapping* &curve,
                        const aString & command = nullString,
			DialogData *interface =NULL );

  int createCurveOnSurface( GenericGraphicsInterface & gi,
			    SelectionInfo & select, 
			    /*PickInfo3D & pick,*/
			    Mapping* &curve );
  
  int computeCellVolumes(const realArray & xt, const int & i3Mod2,
                         real & minCellVolume, real & maxCellVolume, 
			 const real & dSign );
  int equidistribute( const int & i3, realArray & x, const real & weight );
  
  realArray normalize( const realArray & u );
  
  int project( realArray & x, const int & i3,        
               const int & marchingDirection,
               realArray & xr, 
               const bool & setBoundaryConditions= TRUE );
  virtual int project( realArray & x, 
                       MappingProjectionParameters & mpParams ){ return Mapping::project(x,mpParams); }

  
  void plotDirectionArrows(GenericGraphicsInterface & gi, GraphicsParameters & params);

  int plotCellQuality(GenericGraphicsInterface & gi, 
		      GraphicsParameters & parameters);
  
  int printStatistics(FILE *file=stdout );

  int findMatchingBoundaryCurve( int side, int axis );  // find boundary condition curves to match to
  

  bool evaluateTheSurface;   // if TRUE the surface is re-evaluated
  realArray xSurface;        // holds the surface grid 

  // parameters for Bill's generator
  int growthOption;  // +1, -1 or 2=grow in both directions
  real distance[2];
  int linesToMarch[2];
  real upwindDissipationCoefficient, uniformDissipationCoefficient;
  int numberOfVolumeSmoothingIterations;
  real curvatureSpeedCoefficient;
  real implicitCoefficient;
  bool removeNormalSmoothing;
  real equidistributionWeight;
  
  IntegerArray boundaryCondition, projectGhostPoints, indexRange,gridIndexRange,dimension;
  Mapping *boundaryConditionMapping[2][2]; // pointers to Mappings used for a boundray condition.
  bool boundaryConditionMappingWasNewed[2][2]; // true if the mapping was ned'd locally.
  MappingProjectionParameters boundaryConditionMappingProjectionParameters[2][2];
  // surfaceMappingProjectionParameters[0] = reverse, [1]=forward, [2]=for initial curve
  MappingProjectionParameters surfaceMappingProjectionParameters[3]; // for projecting onto a surface grid
    
  int numberOfLinesForNormalBlend[2][2];
  real splayFactor[2][2];   // factor for outward splay BC.

  enum GhostLineOptions
  {
    extrapolateGhostLines,           // ghost line values are extrapolated in the DataPointMapping.
    computeGhostLinesWhenMarching    // compute ghost lines when marching
  } ghostLineOption;  

  realArray gridDensityWeight;   // holds the inverse of the desired grid spacing (relative values only)
  realArray xHyper;              // holds the hyperbolic grid.
  realArray xtHyper;             
  realArray normalCC;            // normal at cell centre.
  
  SpacingType spacingType;
  real geometricFactor;
  real geometricNormalization[2];
  real initialSpacing;

  real minimumGridSpacing;      // stop marching the surface grid when spacing is less than
  real arcLengthWeight, curvatureWeight, normalCurvatureWeight;
  
  real matchToMappingOrthogonalFactor;

  Mapping *normalDistribution;

  bool surfaceGrid;      // true if we are generating a surface grid
  Mapping *startCurve;  // defines the starting curve for a surface grid
  int numberOfPointsOnStartCurve;

  bool projectInitialCurve;  // true if we project starting curve onto the surface

  bool useTriangulation;      // if true, use the triangulation of a CompositeSurface for marching.
  bool projectOntoReferenceSurface;  // if true, project points found using the triangulation onto the actual surface

  int saveReferenceSurface;  // // 0=do not save, 1=save for 2D grids, 2=save for all grids 
  bool plotBoundaryConditionMappings;
  bool plotHyperbolicSurface;
  bool plotObject;
  bool plotDirectionArrowsOnInitialCurve;
  bool plotReferenceSurface;
  bool referenceSurfaceHasChanged;
  bool choosePlotBoundsFromReferenceSurface;
  InitialCurveEnum initialCurveOption;
  real edgeCurveMatchingTolerance; // for deciding when edge curves should be joined.
  
  realArray trailingEdgeDirection;   // for airfoil like trailing edge singularities.

  // variables for the surface grid stuff
  bool smoothAndProject; // if true smooth and project onto original surface definition.
  IntegerArray subSurfaceIndex;
  realArray subSurfaceNormal;
  int numberOfBoundaryCurves;
  Mapping **boundaryCurves;
  bool initialCurveIsABoundaryCurve;

  int totalNumberOfSteps;
  enum TimingsEnum
  {
    totalTime=0,
    timeForProject,
    timeForSetupRHS,
    timeForImplicitSolve,
    timeForTridiagonalSolve,
    timeForFormBlockTridiagonalSystem,
    timeForFormCMatrix,
    timeForNormalAndSurfaceArea,
    timeForSmoothing,
    timeForUpdate,
    timeForBoundaryConditions,
    numberOfTimings
  };
  real timing[numberOfTimings];
  

  // Hypgen parameters
  int nzreg,izstrt,ibcja,ibcjb,ibcka,ibckb,imeth,ivspec,itsvol,iaxis;
  real smu2, epsss, timj,timk , exaxis, volres;
  intArray npzreg;
  RealArray zreg,dz0,dz1;

 private:
  aString className;
  Mapping *surface;       // here is the mapping that we start with
  DataPointMapping *dpm;  // Here is where the mapping is defined.
  char buff[80];

  realArray at,bt,ct,c,lambda;
  
  static FILE *debugFile;

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((HyperbolicMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((HyperbolicMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType copyType = DEEP ) const
      { return ::new HyperbolicMapping(*this, copyType); }

};


#endif  
