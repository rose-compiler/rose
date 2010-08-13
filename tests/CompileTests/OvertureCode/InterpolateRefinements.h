#ifndef INTERPOLATE_REFINEMENTS_H
#define INTERPOLATE_REFINEMENTS_H

#include "Overture.h"
#include "Interpolate.h"

class InterpolateRefinements
{
 public:

  enum
  {
    allLevels=-1
  };
   
  InterpolateRefinements(int numberOfDimensions);
  ~InterpolateRefinements();
  
  // interpolate all values on a new adaptive grid from values on an old (different) adaptive grid.
  int interpolateRefinements( const realGridCollectionFunction & uOld, 
			      realGridCollectionFunction & u,
			      int baseLevel = 1 );

  // interpolate ghost boundaries of refinements from grids at same level or below:
  int interpolateRefinementBoundaries( realGridCollectionFunction & u,
                                       int levelToInterpolate = allLevels,
				       const Range & C = nullRange  );

  // interpolate coarse grid points covered by finer grids:
  int interpolateCoarseFromFine( realGridCollectionFunction & u,
                                 int levelToInterpolate= allLevels,
				 const Range & C = nullRange  );
  

  int get( const GenericDataBase & dir, const aString & name);

  int put( GenericDataBase & dir, const aString & name) const;

  int getIndex( const BOX & box, Index Iv[3] );
  int getIndex( const BOX & box, int side, int axis, Index Iv[3]);
  
  Box intersects( const Box & box1, const Box & box2 );

  int setOrderOfInterpolation( int order );
  int setNumberOfGhostLines( int numberOfGhostLines );
  
  BOX buildBox(Index Iv[3] );

  int debug;

 protected:
  int numberOfDimensions;

  Interpolate interp;
  InterpolateParameters interpParams;
  IntegerArray refinementRatio;
  int numberOfGhostLines;
  
  bool boxWasAdjustedInPeriodicDirection(BOX & box, GridCollection & gc, int baseGrid, int level,
                                         int & periodicDirection, int & periodShift  );

  Box buildBaseBox( MappedGrid & mg );
  
};

#endif
