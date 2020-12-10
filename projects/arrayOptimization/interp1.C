
//#include <A++.h>
#include "simpleA++.h"

void interpolate1D ( floatArray & fineGrid, floatArray & coarseGrid )
   {
     int fineGridSize   = fineGrid.getLength(0);
     int coarseGridSize = coarseGrid.getLength(0);


  // Interior fine points
     Range If (2,fineGridSize-2,2);
     Range Ic (1,coarseGridSize-1,1);

  // separate loops to be fused
     fineGrid(If)    = coarseGrid(Ic);
     fineGrid(If-1)  = (coarseGrid(Ic-1) + coarseGrid(Ic)) / 2.0;
   }

