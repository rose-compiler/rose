#include <A++.h>

void interpolate1D ( floatArray & fineGrid, floatArray & coarseGrid )
   {
     int fineGridSize   = fineGrid.getLength(0);
     int coarseGridSize = coarseGrid.getLength(0);

     Range fineIndexAtCoarseGridPoints      (0,fineGridSize,2);
     Range fineIndexAtInterpolationPoints   (1,fineGridSize-2,2);

     Range coarseIndex                      (0,coarseGridSize,1);
     Range coarseIndexAtInterpolationPoints (0,coarseGridSize-2,1);

     fineGrid(fineIndexAtCoarseGridPoints)     = coarseGrid(coarseIndex);
     fineGrid(fineIndexAtInterpolationPoints) = (coarseGrid(coarseIndexAtInterpolationPoints) + 
                                                  coarseGrid(coarseIndexAtInterpolationPoints+1)) / 2.0;
   }



void work()
   {
     int fineGridSize = 5;
     assert (fineGridSize % 2 == 1);
     int coarseGridSize = (fineGridSize/2)+1;

     floatArray fineGrid   (fineGridSize);
     floatArray coarseGrid (coarseGridSize);

  // Initialization of grids
     fineGrid   = 0.0;
     coarseGrid = 1.0;

     interpolate1D (fineGrid,coarseGrid);
   }

