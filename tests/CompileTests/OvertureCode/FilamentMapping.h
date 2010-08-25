//
// Filament Mapping  August 7, 2000
// **pf
//      $Id: FilamentMapping.h,v 1.1 2004/07/07 10:27:04 dquinlan Exp $

// ADD 9/28/00   **pf
//   "Enter the tangent of the line of translation"
//   "Enter the speed of translation"
//

// Jan 2001 rehash off the FilamentMapping code **pf
//  * new flag 'geometricDataNeedsUpdate' replaces 'filamentNeedsUpdate'
//  * Bdry & motion type enums are renamed/reorganized:
//    >  The filament can be open, closed(=periodic) or a 
//       user defined curve
//       ==> enum FilamentType 
//
//    >  The boundary of the physical filament (in the 'fluid domain')
//       can be
//         ** THICK_FILAMENT -- only for OPEN_FILAMENTS (or user defined, 
//            non, periodic centerlines);
//            .. Make a thick  filament, which is a periodic curve.
//         ** ONE_SIDED_FILAMENT -- for OPEN or CLOSED filaments:
//            .. Use when fluid is only on one side of the filament.
//            .. E.g. closed filament = fluid bubble in air (where air
//               has no dynamics; or a (nondynamic) airbubble in a fluid.
//            .. E.g. 2, an open filament = long bubble moving into a
//               a tube, symmetric interface, a la Reinelt & Saffman. 
//         ** THIN_TWO_SIDED -- not available yet.
//              requires using H-grids, for, e.g. the elastic filament.
//  * Removed fluff
//    > realArray x0 -- not needed, was supposed to have centerline=xFilament
// 
#ifndef FILAMENT_MAPPING_H
#define FILAMENT_MAPPING_H

#include "Overture.h"
#include "Mapping.h"
#include "GenericGraphicsInterface.h"
#include "Display.h"

//..Forward definitions
class HyperbolicMapping;
class SplineMapping;
class GenericGraphicsInterface;
//class DeformingGridGenerationInformation;
class TravelingWaveParameters;

class FilamentMapping : public Mapping 
{

 private:    
  aString    className;
  // Private data representing the Filament

 public:
  
  FilamentMapping(int  nFilamentPoints= 17, 
		  int  nEndPoints     = 3,
		  real thickness      = 0.04,
		  real endRadius      = 0.02
		  );

  ~FilamentMapping();

  //---------------------STANDARD MAPPING INTERFACE
  //Copy constructors
  FilamentMapping( const FilamentMapping &, const CopyType copyType=DEEP );
  FilamentMapping & operator =( const FilamentMapping & xmap00 );

  void map( const realArray & r, realArray & x,
	    realArray & xr = Overture::nullRealDistributedArray(),
	    MappingParameters & params=Overture::nullMappingParameters() );
              
  void basicInverse( const realArray & x, realArray & r, realArray & rx, MappingParameters & params );
  
  virtual int get( const GenericDataBase & dir, const aString & name); 
  virtual int put( GenericDataBase & dir, const aString & name) const; 

  Mapping *make( const aString & mappingClassName );

  aString getClassName() const;

  int update( MappingInformation & mapInfo );

  //
  //------------------Filament functionality
  //

  void replaceHyperbolicMapping( HyperbolicMapping *pNewHyper );
  void evaluateAtTime( real time00 );
  void evaluateSurfaceAtTime( real time0, SplineMapping *curvePosition); 

  //n/a: void generate();  // update filament & regenerate HyperbolicMapping?

  //..Public
  void regenerateBodyFittedMapping();
  void copyBodyFittedMapping( HyperbolicMapping &copyMap, 
			      aString *pMappingRename = NULL);



  //enum PrescribedMotionType {
  //  NO_MOTION,
  //  RIGID_BODY_MOTION,
  //  TRAVELING_WAVE_MOTION,
  //  TRANSLATING_MOTION    /* NEW -- add the functionality for this */
  //};

  //..Filament & Grid types
  //  * Filaments are open or closed(=periodic). Or, a given centerline.
  //  * the boundary can be thick, one-sided, or thin(=H-grid)
  //  * the motion can be RIGID, TRAVELINGWAVE or TRANSLATING
  //
  enum FilamentType {
    OPEN_FILAMENT, 
    CLOSED_FILAMENT,
    USER_DEFINED_CURVE_FILAMENT// was: USER_DEFINED_CENTER_LINE
  };

  enum FilamentBoundaryType {
    THICK_FILAMENT_BOUNDARY,
    ONE_SIDED_FILAMENT_BOUNDARY,
    THIN_TWO_SIDED_FILAMENT_BOUNDARY
  };

  enum BoundaryMotionType {
    NO_MOTION,
    RIGID_BODY_MOTION,
    TRAVELING_WAVE_MOTION,
    TRANSLATING_MOTION
  };
  
  //....   User access routines to parameters: changing 
  //.... requires the filament to be updated
  //....

  void setOffset(                  real xOffset=0.,
                                   real yOffset=0.);
  
  void setLength(                  real length00);

  void setTravelingWaveParameters( real length00=1.,    // length
                                   real aParam00=0.1,   // amplitude param A
				   real bParam00=0.05,  // - "" -  B
				   real omega00=1.2,    // time freq.
				   real knum00=1.2      // space freq
				   );

  void setTranslatingMotionParameters( real tangX, real tangY, real velo);
  void getTranslatingMotionParameters( real &tangX, real &tangY, real &velo);

  void setFilamentTime( real newTime );
  void setFilamentTimeOffset( real newTime );
  void setThicknessAndEndRadius( real thick00, real endRadius00);

  void setNumberOfFilamentPoints( int nFilamentPoints00, int nSplinePoints ); //equal by default
  void setNumberOfFilamentPoints( int nFilamentPoints00);
  
  void setNumberOfEndPoints( int nEndPoints00=5 );
  void setNumberOfThickFilamentPoints( int nThickSplinePoints ); //by default =2*nFilamPoints
  void setNumberOfThickFilamentPoints();

  //old:   void setMotionType( PrescribedMotionType motionType00);
  void setMotionType( BoundaryMotionType motionType00);
  void setFilamentType(  FilamentType filamentType00);
  void setFilamentBoundaryType(FilamentBoundaryType filamBdryType00);
  void setHyperbolicMappingParameters( real distanceToMarch00         = 0.3,
                                       real dissipation00             = 0.2);

  void setHyperbolicGridDimensions(    int gridDimension1_,    int gridDimension2_,
                                       int gridGenDimension1_, int gridGenDimension2_);

  void setHyperbolicGridInfo();
  void getHyperbolicGridInfo();

  HyperbolicMapping* getHyperbolicMappingPointer();
  void releaseHyperbolicMappingPointer(HyperbolicMapping* &pHyp00);
  void regenerateBodyFittedMapping(  HyperbolicMapping *pHyper);

  //--PARAMETERS -- set by the user
  int            nFilamentPoints;
  int            nSplinePoints;               //  >= nFilamPts

  int            nThickSplinePoints;
  int            nEndPoints;

  real           filamentLength; 
  real           xOffset, yOffset;            // filament leading edge
  real           sXDirection, sYDirection;    // tang. direction of lead. edge

  real thickness;                             // thickness
  real endRadius;                             // radius of the end

  int            debug;                       // DEBUG level

  real           timeOffset;                  // tFilament = tcomp-timeOffset

  //..FILAMENT DATA
//   enum FilamentType {
//     OPEN_FILAMENT, 
//     CLOSED_FILAMENT,
//     USER_DEFINED_CURVE_FILAMENT// was: USER_DEFINED_CENTER_LINE
//   };

//   enum FilamentBoundaryType {
//     THICK_FILAMENT_BOUNDARY,
//     ONE_SIDED_FILAMENT_BOUNDARY,
//     THIN_TWO_SIDED_FILAMENT_BOUNDARY
//   };

//   enum BoundaryMotionType {
//     NO_MOTION,
//     RIGID_BODY_MOTION,
//     TRAVELING_WAVE_MOTION,
//     TRANSLATING_MOTION
//   };
  

  //....Closed filament data
  //  =OPEN_FILAMENT, CLOSED_FILAMENT, USER_DEFINED_CENTER_LINE  
  //enum BoundaryMotionType {
  //  NO_MOTION,
  //  DESCRIBED_BY_CENTERLINE_MOTION,
  //  RIGID_BODY_MOTION,
  //  TRAVELING_WAVE_MOTION
  //  TRANSLATING_MOTION    /* NEW -- add the functionality for this */
  //};
  FilamentType         filamentType;  
  BoundaryMotionType   motionType;
  FilamentBoundaryType filamentBoundaryType;

  //....Traveling wave parameters
  real           aTravelingWaveAmplitude,      // amplitude parameters
                 bTravelingWaveAmplitude; 
  real           timeFrequencyTravelingWave,   // time & space freq. 
                 spaceFrequencyTravelingWave; 
  real           timeForFilament;              // eval. filament at this time


  real           radius;
  real           perturbation1, perturbation2;
  int            circleMode1,   circleMode2;
  real           circlePhase1,  circlePhase2;

  //....Rigid body motion data
  real           xMotionDirection, yMotionDirection;  // direction of movement
  real           motionVelocity;                      // speed of movement
  real           xOffsetCurrent, yOffsetCurrent;
  real           rotationAngleCurrent;
  real           rotationSpeed;

  //..Working Data
  realArray x,r,xr;
  realArray sr;        // arc-length deriv.
  realArray amplitude, dAmplitude;

  bool isFilamentInitialized;        // set to FALSE by initializeFilamentStorage()
  bool geometricDataNeedsUpdate;     // set to FALSE by computeGeometricData()
  bool bodyFittedMappingNeedsUpdate; // params for HyperbMapping have changed

  //..Centerline = user defined core from which filament is formed
  Mapping *pUserDefinedCoreCurve;     // was: *pCenterLineMapping;

  //REPRESENTATION as splines & normal/tang vecs
  SplineMapping *pFilament;                       // The filament=Core

  //..Thick Filament = bdry with fluid
  SplineMapping *pThickFilament;           // bdry with fluid
  int       nTotalThickFilamentPoints;


  //POSITION
  realArray      normalVector, tangentVector;     // Normal & Tangent
  realArray xFilament;                            // filament (x,y) values

  //realArray x0;                          // the centerline? NOT USED?

  realArray xTop,xBottom;                // Pieces of the thick filam.
  realArray xLeadingEnd, xTrailingEnd;   // - " -

  realArray xThickFilament;              // All of thickFilam->to Spline

  //
  //-------------DeformingGrid generator--Body fitted mapping
  //
  int numberOfDimensions;  // always =2 (i.e. 2D only)
  HyperbolicMapping *pHyper;

  Mapping *getSurfaceForHyperbolicMap(); // return the surface curve
  //         This is =pThickFilament for thick filam, 
  //         and     =pFilament for one-sided interfaces
  //



  //
  //--------------INTERFACE TO THE FILAMENT
  //
  void constructor(  int nFilamentPoints00, 
                     int nEndPoints00,      
		     real thickness00,     
		     real endRadius00 );

  void setDefaultHyperbolicParameters();
 
  void initializeFilamentStorage();
  void initializeBodyFittedMapping( Mapping *pSurface);
  void initializeBodyFittedMapping( );

  bool updateFilamentAndGrid();

  void formNormalizedParametrization( realArray r00 );
  void computeGeometricData();
  void computeThickFilament();


  void setCenterLineMapping( Mapping *pMapping );

  void displayParameters();

  //DEBUG DATA
  Display    *pDisplay;
  realArray   xDebug;

  //--USER DEFINED CENTERLINE
   void computeUserDefinedCenterLineFilament( 
			     real t,        realArray & r, 
			     realArray & x, realArray & xr );
 
  //--RIGID BODY DYNAMICS

  void computeTranslatingMotionOffset( real time00,
                                       real xOffset00, real yOffset00,
				       real &xNew,     real &yNew);

  // ..not available yet

  //--TRAVELING WAVE DYNAMICS

  //..parameters -- OBSOLETE, not used..
  TravelingWaveParameters *pTravelingWaveData;

  //
  // N.B: choose wavespeed = fluid speed = -1
  //    * HENCE omega/knum = 1 is appropriate
  //    * increase space freq (=knum) by 1.2 to make it wigglier
  //    * therefore omega = 1.2
  //      (choosing knum=2 with sparse grid is too wiggly)
  //


  

  void computeTravelingWaveFilament( real t, realArray & r, 
                                     realArray & x, realArray & xr );

  void computeTravelingWaveFilament( real t, realArray & r, 
                                     realArray & x, realArray & xr,
				     real xOffset00, real yOffset00);
  
  void computeTimeDerivativesForTravelingWaveFilament
            ( real t,            realArray & r00, 
	      realArray & x00,   realArray &xr00,
	      realArray & xt00,  realArray &xtt00,
	      realArray & xrt00, realArray &xrtt00);

  void computeCircularFilament(  real tcomp, realArray & r00, 
                                 realArray & x00, realArray &xr00,
				 real xOffset00 =0., real yOffset00=0. );

  void printHyperbolicDimensions();

 private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& xmap)
    { return operator=( (FilamentMapping &)xmap); }
    virtual void reference( const ReferenceCounting& xmap)
      { reference( (FilamentMapping &)xmap); }
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP )
      const
      { return ::new FilamentMapping(*this, ct); }

};

#endif                 
