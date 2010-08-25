#ifndef DynamicMovingGrids_h
#define DynamicMovingGrids_h

#include <Overture.h>
#include "GenericGridMotion.h"
//#include "Cgsh.h" GONE IN V16
#include "Ogen.h"

class DynamicMovingGrids 
{
//
//\begin{>DynamicMovingGrids.tex}{\subsection{\DynamicMovingGrids class}}
//\no function header:
//    This class provides a uniform interface for grid motion to the
//    user's PDE solver within the Overture Framework.
// /Author: D L Brown 980930
//
//\end{DynamicMovingGrids.tex}

   public:
  
      DynamicMovingGrids ();

      DynamicMovingGrids (PlotStuff* ps_);

  //... GONE in V16
//      DynamicMovingGrids (CompositeGrid& compositeGrid_, 
//			  GenericGridMotion* genericGridMotion_,
//			  Cgsh* gridGenerator_,
//			  PlotStuff* ps_ = NULL);

      //... Here's another constructor using ogen instead of cgsh
      DynamicMovingGrids (CompositeGrid& compositeGrid_, 
                          GenericGridMotion* genericGridMotion_,
			  Ogen* ogenGridGenerator_,
			  PlotStuff* ps_ = NULL);

//... GONE in V16
//      void initialize (CompositeGrid& compositeGrid_,
//		       GenericGridMotion* genericGridMotion,
//		       Cgsh* gridGenerator_);

      void initialize (CompositeGrid& compositeGrid_,
                       GenericGridMotion* genericGridMotion,
		       Ogen* ogenGridGenerator_);

   private:
      void initialize (CompositeGrid& compositeGrid_,
                       GenericGridMotion* genericGridMotion);
   public:
  
      virtual ~DynamicMovingGrids ();

      //...return list of pointers to all the moved grids
      CompositeGrid** movedGrids ();

      //...return pointer to just one grid (at a specified level)
      CompositeGrid* movedGrid (const int & level);

      //...move the grids, recompute overlap
      
      void updateMovedGrids (const real & time,
                             const real & timestep);

      //...eventually there needs to be an interface for the nonlinear case (when
      //   the grid motion depends on the solution), but I
      //   haven't figured out yet what that should look like.

      //...return a pointer to the grid velocity function for the level'th grid; this can call
      //   a function in GenericGridMotion, or compute it by differencing
      void getGridVelocity (realCompositeGridFunction& velocity,
                            const int & level,
			    const real & time);

      void getGridBoundaryVelocity (realCompositeGridFunction& velocity,
				    const int & level,
				    const real & time);
  

   protected:
      //...objects of this type should at most be passed by reference,
      //   never copied
      //...by protecting the copy constructor and operator=, we prevent
      //   them from ever being called; 
      DynamicMovingGrids (const DynamicMovingGrids& dmg);  
      DynamicMovingGrids& operator=(const DynamicMovingGrids& dmg);

      CompositeGrid** cgMoving;
      GenericGridMotion* genericGridMotion;
//    Cgsh* gridGenerator;  GONE in V16
      Ogen* ogenGridGenerator;
      
      PlotStuff* ps;
      PlotStuffParameters psp;

      int numberOfLevels;
      int numberOfGrids;
      int numberOfDimensions;

      bool mainConstructorCalled;
      bool compositeGridListInitialized;
      bool movingGridMappingsSet;
      bool firstTimeToMoveGrids;
      bool gridsHaveMoved;  //...true if updateMovedGrids has been called at least once
      
      LogicalArray hasMoved;
      int whatToUpdate;

      bool useOgen;

      void initializeCompositeGridList (CompositeGrid& compositeGrid_);
      void destructCompositeGridList ();

      //...get Mapping's from the GenericGridMotion object and use them to
      //   replace the ones in the moving grids
      void setMovingGridMappings ();
      void swapCompositeGrids ();

};


#endif












