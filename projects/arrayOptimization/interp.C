
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

void interpolate2D ( floatArray & fineGrid, floatArray & coarseGrid )
   {
     int fineGridSizeX   = fineGrid.getLength(0);
     int fineGridSizeY   = fineGrid.getLength(1);
     int coarseGridSizeX = coarseGrid.getLength(0);
     int coarseGridSizeY = coarseGrid.getLength(1);

  // Interior fine grid points
     Range If (2,fineGridSizeX-2,2);
     Range Jf (2,fineGridSizeY-2,2);

     Range Ic (1,coarseGridSizeX,1);
     Range Jc (1,coarseGridSizeY-1,1);

  // separate loops to be fused
     fineGrid(If,Jf)     =  coarseGrid(Ic,Jc);
     fineGrid(If-1,Jf)   = (coarseGrid(Ic-1,Jc) + coarseGrid(Ic,Jc)) / 2.0;
     fineGrid(If,Jf-1)   = (coarseGrid(Ic,Jc-1) + coarseGrid(Ic,Jc)) / 2.0;
     fineGrid(If-1,Jf-1) = (coarseGrid(Ic-1,Jc-1) + coarseGrid(Ic-1,Jc) + 
                            coarseGrid(Ic,Jc-1) + coarseGrid(Ic,Jc)) / 4.0;
   }

void redBlackRelax1D ( floatArray & s, floatArray & rhs )
   {
     int gridSize = s.getLength(0);
     Index Black_Odd  ( 1 , (gridSize - 1) / 2 , 2 );
     Index Black_Even ( 2 , (gridSize - 2) / 2 , 2 );

     Index Red_Odd    ( 1 , (gridSize - 1) / 2 , 2 );
     Index Red_Even   ( 2 , (gridSize - 2) / 2 , 2 );

     Index Odd_Rows   ( 1 , (gridSize - 1) / 2 , 2 );
     Index Even_Rows  ( 2 , (gridSize - 2) / 2 , 2 );

     double hSqr = 1.0 / double(gridSize * gridSize);

  // separate loops to be fused
     s(Black_Even) = ( hSqr * rhs(Black_Even) + s(Black_Even+1) + s(Black_Even-1) ) / 2.0;
     s(Red_Odd)    = ( hSqr * rhs(Red_Odd)    + s(Red_Odd+1)    + s(Red_Odd-1)    ) / 2.0;
   }

void redBlackRelax2D ( floatArray & s, floatArray & rhs )
   {
     int gridSize = s.getLength(0);
     Index Black_Odd  ( 1 , (gridSize - 1) / 2 , 2 );
     Index Black_Even ( 2 , (gridSize - 2) / 2 , 2 );

     Index Red_Odd    ( 1 , (gridSize - 1) / 2 , 2 );
     Index Red_Even   ( 2 , (gridSize - 2) / 2 , 2 );

     Index Odd_Rows   ( 1 , (gridSize - 1) / 2 , 2 );
     Index Even_Rows  ( 2 , (gridSize - 2) / 2 , 2 );

     double hSqr = 1.0 / double(gridSize * gridSize);

  // separate loops to be fused
     s(Black_Odd,Odd_Rows)   = ( hSqr * rhs(Black_Odd,Odd_Rows) + 
                                 s(Black_Odd+1,Odd_Rows) + s(Black_Odd-1,Odd_Rows) + 
                                 s(Black_Odd,Odd_Rows+1) + s(Black_Odd,Odd_Rows-1) ) / 4.0;
     s(Black_Even,Even_Rows) = ( hSqr * rhs(Black_Even,Even_Rows) + 
                                 s(Black_Even+1,Even_Rows) + s(Black_Even-1,Even_Rows) + 
                                 s(Black_Even,Even_Rows+1) + s(Black_Even,Even_Rows-1) ) / 4.0;
     s(Red_Even,Odd_Rows)    = ( hSqr * rhs(Red_Even,Odd_Rows) + 
                                 s(Red_Even+1,Odd_Rows) + s(Red_Even-1,Odd_Rows) + 
                                 s(Red_Even,Odd_Rows+1) + s(Red_Even,Odd_Rows-1) ) / 4.0;
     s(Red_Odd,Even_Rows)    = ( hSqr * rhs(Red_Odd,Even_Rows) + 
                                 s(Red_Odd+1,Even_Rows) + s(Red_Odd-1,Even_Rows) + 
                                 s(Red_Odd,Even_Rows+1) + s(Red_Odd,Even_Rows-1) ) / 4.0;
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

     floatArray solution (fineGridSize);
     floatArray rhs      (fineGridSize);

     rhs      = 0.0;
     solution = 1.0;

     redBlackRelax1D (solution,rhs);     
   }



