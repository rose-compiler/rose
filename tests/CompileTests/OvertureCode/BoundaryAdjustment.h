#ifndef BOUNDARY_ADJUSTMENT_H
#define BOUNDARY_ADJUSTMENT_H

// The BoundaryAdjustment object stores the information needed to
// adjust the boundaries of a grid with respect to a second grid. 
// This is basically a stupid container class. The grid generator knows 
// how to fill in the values into the data members of this class.

#include "GridCollection.h"
#include "CompositeGridFunction.h"

class 
BoundaryAdjustment 
{
 public:
  enum SidesShareEnum
  {
    doNotShare=0,
    share=1,
    unknown,
    shareButNoAdjustmentNeeded
  };

  
  BoundaryAdjustment();
  ~BoundaryAdjustment();
  
  BoundaryAdjustment& operator=(const BoundaryAdjustment& x);
  BoundaryAdjustment(const BoundaryAdjustment& x);

  int reference( const BoundaryAdjustment& x);

  int create();
  int destroy();

  RealMappedGridFunction & boundaryAdjustment(){ assert(ba!=NULL); return *ba; }
  RealMappedGridFunction & acrossGrid(){ assert(ag!=NULL); return *ag;}
  RealMappedGridFunction & oppositeBoundary(){ assert(ob!=NULL); return *ob;}
  IntegerArray & sidesShare(){ assert(ss!=NULL); return *ss;}

  // Are there any shared sides:
  SidesShareEnum hasSharedSides() const {return (SidesShareEnum)sharedSidesExist;} 
  SidesShareEnum & hasSharedSides() {return (SidesShareEnum &)sharedSidesExist;} 

  // is a face shared with (side,axis) of the other grid:
  SidesShareEnum sidesShare(int side, int axis) const;


  // return true if the grid functions and arrays have been built.
  bool wasCreated() const { return ba!=NULL; } //
  
  int computedGeometry;  // put for backward compatibility
  
 protected:
  RealMappedGridFunction *ba, *ag, *ob;
  IntegerArray *ss;
  int sharedSidesExist;
  
};

#endif
