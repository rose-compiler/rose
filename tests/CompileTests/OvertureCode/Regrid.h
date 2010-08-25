#ifndef REGRID_H
#define REGRID_H

#include "Overture.h"
#include "BoxLib.H"
#include "BoxList.H"

class RotatedBox;
class ListOfRotatedBox;
class GenericGraphicsInterface;

class Regrid
{
 public:

  enum GridAdditionOption
  {
    addGridsAsRefinementGrids,
    addGridsAsBaseGrids
  };

  enum GridAlgorithmOption
  {
    aligned,
    rotated
  };
  
  Regrid();
  ~Regrid();
  
  int getDefaultNumberOfRefinementLevels() const;
  int getRefinementRatio() const;

  int displayParameters(FILE *file = stdout ) const;

  // regrid based on an error mask
  int regrid( GridCollection & gc,             // grid to regrid.
	      GridCollection & gcNew,          // put new grid here (must be different from gc)
	      intGridCollectionFunction & errorMask,  // =1 at points to refine
	      int refinementLevel = 1,  // highest level to refine
	      int baseLevel  = -1 );    // keep this level and below fixed, by default baseLevel=refinementLevel-1.

  // regrid based on an error function and error tolerance
  int regrid( GridCollection & gc,             // grid to regrid.
	      GridCollection & gcNew,          // put new grid here (must be different from gc)
	      realGridCollectionFunction & error,
	      real errorThreshhold,
	      int refinementLevel = 1,  // highest level to refine
	      int baseLevel  = -1 );    // keep this level and below fixed, by default baseLevel=refinementLevel-1.

  int outputRefinementInfo( GridCollection & gc, 
			    const aString & gridFileName, 
			    const aString & fileName );

  int printStatistics( GridCollection & gc, FILE *file = NULL, 
                       int *numberOfGridPoints=NULL );
  
  void setEfficiency(real efficiency);  // gridding efficiency, 0 < efficiency < 1

  void setNumberOfBufferZones(int numberOfBufferZones);     // exapansion of tagged error points.

  void setWidthOfProperNesting( int widthOfProperNesting ); // distance between levels

  void setRefinementRatio( int refinementRatio );

  void setUseSmartBisection( bool trueOrFalse=true );

  void setGridAdditionOption( GridAdditionOption gridAdditionOption );

  void setGridAlgorithmOption( GridAlgorithmOption gridAlgorithmOption );

  void setMaximumNumberOfSplits( int num );
  
  void setMergeBoxes( bool trueOrFalse=true ); // allow boxes to be merged?
  
  int update( GenericGraphicsInterface & gi );

  int get( const GenericDataBase & dir, const aString & name);

  int put( GenericDataBase & dir, const aString & name) const;

  int debug;

 protected:

  enum CutStatus
  {
    invalidCut,
    holeCut,
    steepCut,
    bisectCut
  };

  int buildProperNestingDomains(GridCollection & gc, 
				int baseGrid,
				int refinementLevel,
				int baseLevel,
				int numberOfRefinementLevels   );
  
  int buildTaggedCells(MappedGrid & mg, 
                       intMappedGridFunction & tag, 
                       const realArray & error, 
                       real errorThreshhold,
                       bool useErrorMask,
		       bool cellCentred = true );

  Box cellCenteredBox( MappedGrid & mg, int ratio=1 );
  Box cellCenteredBaseBox( MappedGrid & mg );
  
  int findCut(int *hist, int lo, int hi, CutStatus &status);
  int findCutPoint( BOX & box, const intArray & ia, int & cutDirection, int & cutPoint );
  int fixPeriodicBox( MappedGrid & mg, BOX & mainBox, const intArray & ia, int level );
  
  BOX getBox( const intArray & ia );
  BOX buildBox(Index Iv[3] );
  
  real getEfficiency(const intArray & ia, const BOX & box );
  
  int buildGrids( GridCollection & gc, 
                  GridCollection & gcNew,
                  int baseGrid, int baseLevel, int refinementLevel, BoxList *refinementBoxList);
  
  int regridAligned( GridCollection & gc, 
                     GridCollection & gcNew, 
                     bool useErrorFunction,
		     realGridCollectionFunction *pError,
		     real errorThreshhold,
		     intGridCollectionFunction & tagCollection,
		     int refinementLevel = 1,
		     int baseLevel  = -1 );  

  int regridRotated( GridCollection & gc, 
                     GridCollection & gcNew, 
                     bool useErrorFunction,
		     realGridCollectionFunction *pError,
		     real errorThreshhold,
		     intGridCollectionFunction & tagCollection,
		     int refinementLevel = 1,
		     int baseLevel  = -1 );  

  int splitBox( BOX & box, const intArray & ia, BoxList & boxList, int refinementLevel );
  int splitBoxRotated( RotatedBox & box, ListOfRotatedBox & boxList, 
                       realArray & xa, int refinementLevel );

  int merge( ListOfRotatedBox & boxList );
  
  int defaultNumberOfRefinementLevels;
  real efficiency;
  int refinementRatio;
  int numberOfBufferZones;
  int widthOfProperNesting;
  int maximumNumberOfSplits;  // limit max no of splits for testing
  int splitNumber;
  
  int minimumBoxSize;
  bool useSmartBisection;
  bool mergeBoxes;

  GridAdditionOption gridAdditionOption;
  GridAlgorithmOption gridAlgorithmOption;
  
  BoxList *properNestingDomain;
  BoxList *complementOfProperNestingDomain;


};


#endif
