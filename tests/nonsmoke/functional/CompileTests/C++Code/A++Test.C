// Hi Qing,
//    I have written the interpolation and red-black relaxation 
//    for 1 and 2 dimensional problems below.  I have commented
//    where the loop fusion is required to get better performance.
//    We can discuss the example tomorrow morning.  The code 
//    compiles with the A++ header files, and likely with your simpler
//    header file (since it does not use many A++ features).

#include <A++.h>

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

void interpolate3D ( floatArray & fineGrid, floatArray & coarseGrid )
   {
     int fineGridSizeX   = fineGrid.getLength(0);
     int fineGridSizeY   = fineGrid.getLength(1);
     int fineGridSizeZ   = fineGrid.getLength(2);
     int coarseGridSizeX = coarseGrid.getLength(0);
     int coarseGridSizeY = coarseGrid.getLength(1);
     int coarseGridSizeZ = coarseGrid.getLength(2);

  // Interior fine grid points
     Range If (2,fineGridSizeX-2,2);
     Range Jf (2,fineGridSizeY-2,2);
     Range Kf (2,fineGridSizeZ-2,2);

     Range Ic (1,coarseGridSizeX,1);
     Range Jc (1,coarseGridSizeY-1,1);
     Range Kc (1,coarseGridSizeZ-1,1);

     fineGrid (If+1,Jf,Kf)     = ( coarseGrid(Ic,Jc,Kc) + coarseGrid(Ic+1,Jc,Kc) ) / 2.0;
     fineGrid (If,Jf+1,Kf)     = ( coarseGrid(Ic,Jc,Kc) + coarseGrid(Ic,Jc+1,Kc) ) / 2.0;
     fineGrid (If,Jf,Kf+1)     = ( coarseGrid(Ic,Jc,Kc) + coarseGrid(Ic,Jc,Kc+1) ) / 2.0;
               
     fineGrid (If+1,Jf,Kf+1)   = ( coarseGrid(Ic,Jc,Kc)   + coarseGrid(Ic+1,Jc,Kc) +
                                   coarseGrid(Ic,Jc,Kc+1) + coarseGrid(Ic+1,Jc,Kc+1) ) / 4.0;
     fineGrid (If,Jf+1,Kf+1)   = ( coarseGrid(Ic,Jc,Kc)   + coarseGrid(Ic,Jc+1,Kc) +
                                   coarseGrid(Ic,Jc,Kc+1) + coarseGrid(Ic,Jc+1,Kc+1) ) / 4.0;
     fineGrid (If+1,Jf+1,Kf)   = ( coarseGrid(Ic,Jc,Kc)   + coarseGrid(Ic,Jc+1,Kc) +
                                   coarseGrid(Ic+1,Jc,Kc) + coarseGrid(Ic+1,Jc+1,Kc) ) / 4.0;

     fineGrid (If  ,Jf  ,Kf  ) =   coarseGrid(Ic,Jc,Kc);
     fineGrid (If+1,Jf+1,Kf+1) = ( coarseGrid(Ic,Jc,Kc)     + coarseGrid(Ic,Jc+1,Kc) +
                                   coarseGrid(Ic+1,Jc,Kc)   + coarseGrid(Ic+1,Jc+1,Kc) +
                                   coarseGrid(Ic,Jc,Kc+1)   + coarseGrid(Ic,Jc+1,Kc+1) +
                                   coarseGrid(Ic+1,Jc,Kc+1) + coarseGrid(Ic+1,Jc+1,Kc+1) ) / 8.0;
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


void redBlackRelax3DNonFullyVariational ( floatArray & s, floatArray & rhs )
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
     s(Black_Odd,Odd_Rows,Odd_Rows) = 
          ( hSqr * rhs(Black_Odd,Odd_Rows,Odd_Rows) + 
               s(Black_Odd+1,Odd_Rows,Odd_Rows) + s(Black_Odd-1,Odd_Rows,Odd_Rows) + 
               s(Black_Odd,Odd_Rows+1,Odd_Rows) + s(Black_Odd,Odd_Rows-1,Odd_Rows) + 
               s(Black_Odd,Odd_Rows,Odd_Rows+1) + s(Black_Odd,Odd_Rows,Odd_Rows-1) ) / 6.0;
     s(Black_Even,Even_Rows,Odd_Rows) = 
          ( hSqr * rhs(Black_Even,Even_Rows,Odd_Rows) + 
               s(Black_Even+1,Even_Rows,Odd_Rows) + s(Black_Even-1,Even_Rows,Odd_Rows) + 
               s(Black_Even,Even_Rows+1,Odd_Rows) + s(Black_Even,Even_Rows-1,Odd_Rows) + 
               s(Black_Even,Even_Rows,Odd_Rows+1) + s(Black_Even,Even_Rows,Odd_Rows-1) ) / 6.0;
     s(Black_Odd,Odd_Rows,Even_Rows) = 
          ( hSqr * rhs(Black_Odd,Odd_Rows,Even_Rows) + 
               s(Black_Odd+1,Odd_Rows,Even_Rows) + s(Black_Odd-1,Odd_Rows,Even_Rows) + 
               s(Black_Odd,Odd_Rows+1,Even_Rows) + s(Black_Odd,Odd_Rows-1,Even_Rows) + 
               s(Black_Odd,Odd_Rows,Even_Rows+1) + s(Black_Odd,Odd_Rows,Even_Rows-1) ) / 6.0;
     s(Black_Even,Even_Rows,Even_Rows) = 
          ( hSqr * rhs(Black_Even,Even_Rows,Even_Rows) + 
               s(Black_Even+1,Even_Rows,Even_Rows) + s(Black_Even-1,Even_Rows,Even_Rows) + 
               s(Black_Even,Even_Rows+1,Even_Rows) + s(Black_Even,Even_Rows-1,Even_Rows) + 
               s(Black_Even,Even_Rows,Even_Rows+1) + s(Black_Even,Even_Rows,Even_Rows-1) ) / 6.0;
     s(Red_Even,Odd_Rows,Odd_Rows) = 
          ( hSqr * rhs(Red_Even,Odd_Rows,Odd_Rows) + 
               s(Red_Even+1,Odd_Rows,Odd_Rows) + s(Red_Even-1,Odd_Rows,Odd_Rows) + 
               s(Red_Even,Odd_Rows+1,Odd_Rows) + s(Red_Even,Odd_Rows-1,Odd_Rows) + 
               s(Red_Even,Odd_Rows,Odd_Rows+1) + s(Red_Even,Odd_Rows,Odd_Rows-1) ) / 6.0;
     s(Red_Odd,Even_Rows,Odd_Rows) = 
          ( hSqr * rhs(Red_Odd,Even_Rows,Odd_Rows) + 
               s(Red_Odd+1,Even_Rows,Odd_Rows) + s(Red_Odd-1,Even_Rows,Odd_Rows) + 
               s(Red_Odd,Even_Rows+1,Odd_Rows) + s(Red_Odd,Even_Rows-1,Odd_Rows) + 
               s(Red_Odd,Even_Rows,Odd_Rows+1) + s(Red_Odd,Even_Rows,Odd_Rows-1) ) / 6.0;
     s(Red_Even,Odd_Rows,Even_Rows) = 
          ( hSqr * rhs(Red_Even,Odd_Rows,Even_Rows) +
               s(Red_Even+1,Odd_Rows,Even_Rows) + s(Red_Even-1,Odd_Rows,Even_Rows) + 
               s(Red_Even,Odd_Rows+1,Even_Rows) + s(Red_Even,Odd_Rows-1,Even_Rows) + 
               s(Red_Even,Odd_Rows,Even_Rows+1) + s(Red_Even,Odd_Rows,Even_Rows-1) ) / 6.0;
     s(Red_Odd,Even_Rows,Even_Rows) = 
          ( hSqr * rhs(Red_Odd,Even_Rows,Even_Rows) + 
               s(Red_Odd+1,Even_Rows,Even_Rows) + s(Red_Odd-1,Even_Rows,Even_Rows) + 
               s(Red_Odd,Even_Rows+1,Even_Rows) + s(Red_Odd,Even_Rows-1,Even_Rows) + 
               s(Red_Odd,Even_Rows,Even_Rows+1) + s(Red_Odd,Even_Rows,Even_Rows-1) ) / 6.0;
   }


void redBlackRelax3DFullyVariational ( floatArray & s, floatArray & rhs )
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
     s(Black_Odd,Odd_Rows,Odd_Rows) = 
          ( hSqr * rhs(Black_Odd,Odd_Rows,Odd_Rows) + 
               s(Black_Odd+1,Odd_Rows,Odd_Rows-1)   + s(Black_Odd-1,Odd_Rows,Odd_Rows-1)   + 
               s(Black_Odd,Odd_Rows+1,Odd_Rows-1)   + s(Black_Odd,Odd_Rows-1,Odd_Rows-1)   + 
               s(Black_Odd+1,Odd_Rows+1,Odd_Rows-1) + s(Black_Odd+1,Odd_Rows-1,Odd_Rows-1) + 
               s(Black_Odd-1,Odd_Rows+1,Odd_Rows-1) + s(Black_Odd-1,Odd_Rows-1,Odd_Rows-1) +
               s(Black_Odd+1,Odd_Rows,Odd_Rows)     + s(Black_Odd-1,Odd_Rows,Odd_Rows)     + 
               s(Black_Odd,Odd_Rows+1,Odd_Rows)     + s(Black_Odd,Odd_Rows-1,Odd_Rows)     + 
               s(Black_Odd+1,Odd_Rows+1,Odd_Rows)   + s(Black_Odd+1,Odd_Rows-1,Odd_Rows)   + 
               s(Black_Odd-1,Odd_Rows+1,Odd_Rows)   + s(Black_Odd-1,Odd_Rows-1,Odd_Rows)   +
               s(Black_Odd+1,Odd_Rows,Odd_Rows+1)   + s(Black_Odd-1,Odd_Rows,Odd_Rows+1)   + 
               s(Black_Odd,Odd_Rows+1,Odd_Rows+1)   + s(Black_Odd,Odd_Rows-1,Odd_Rows+1)   + 
               s(Black_Odd+1,Odd_Rows+1,Odd_Rows+1) + s(Black_Odd+1,Odd_Rows-1,Odd_Rows+1) + 
               s(Black_Odd-1,Odd_Rows+1,Odd_Rows+1) + s(Black_Odd-1,Odd_Rows-1,Odd_Rows+1) +
               s(Black_Odd,Odd_Rows,Odd_Rows-1)     + s(Black_Odd,Odd_Rows,Odd_Rows+1) ) / 26.0;
     s(Black_Even,Even_Rows,Odd_Rows) = 
          ( hSqr * rhs(Black_Even,Even_Rows,Odd_Rows) + 
               s(Black_Even+1,Even_Rows,Odd_Rows-1)   + s(Black_Even-1,Even_Rows,Odd_Rows-1)   + 
               s(Black_Even,Even_Rows+1,Odd_Rows-1)   + s(Black_Even,Even_Rows-1,Odd_Rows-1)   + 
               s(Black_Even+1,Even_Rows+1,Odd_Rows-1) + s(Black_Even+1,Even_Rows-1,Odd_Rows-1) + 
               s(Black_Even-1,Even_Rows+1,Odd_Rows-1) + s(Black_Even-1,Even_Rows-1,Odd_Rows-1) +
               s(Black_Even+1,Even_Rows,Odd_Rows)     + s(Black_Even-1,Even_Rows,Odd_Rows)     + 
               s(Black_Even,Even_Rows+1,Odd_Rows)     + s(Black_Even,Even_Rows-1,Odd_Rows)     + 
               s(Black_Even+1,Even_Rows+1,Odd_Rows)   + s(Black_Even+1,Even_Rows-1,Odd_Rows)   + 
               s(Black_Even-1,Even_Rows+1,Odd_Rows)   + s(Black_Even-1,Even_Rows-1,Odd_Rows)   +
               s(Black_Even+1,Even_Rows,Odd_Rows+1)   + s(Black_Even-1,Even_Rows,Odd_Rows+1)   + 
               s(Black_Even,Even_Rows+1,Odd_Rows+1)   + s(Black_Even,Even_Rows-1,Odd_Rows+1)   + 
               s(Black_Even+1,Even_Rows+1,Odd_Rows+1) + s(Black_Even+1,Even_Rows-1,Odd_Rows+1) + 
               s(Black_Even-1,Even_Rows+1,Odd_Rows+1) + s(Black_Even-1,Even_Rows-1,Odd_Rows+1) +
               s(Black_Even,Even_Rows,Odd_Rows-1)     + s(Black_Even,Even_Rows,Odd_Rows+1) ) / 26.0;
     s(Black_Odd,Odd_Rows,Even_Rows) = 
          ( hSqr * rhs(Black_Odd,Odd_Rows,Even_Rows) + 
               s(Black_Odd+1,Odd_Rows,Even_Rows-1)   + s(Black_Odd-1,Odd_Rows,Even_Rows-1)   + 
               s(Black_Odd,Odd_Rows+1,Even_Rows-1)   + s(Black_Odd,Odd_Rows-1,Even_Rows-1)   + 
               s(Black_Odd+1,Odd_Rows+1,Even_Rows-1) + s(Black_Odd+1,Odd_Rows-1,Even_Rows-1) + 
               s(Black_Odd-1,Odd_Rows+1,Even_Rows-1) + s(Black_Odd-1,Odd_Rows-1,Even_Rows-1) +
               s(Black_Odd+1,Odd_Rows,Even_Rows)     + s(Black_Odd-1,Odd_Rows,Even_Rows)     + 
               s(Black_Odd,Odd_Rows+1,Even_Rows)     + s(Black_Odd,Odd_Rows-1,Even_Rows)     + 
               s(Black_Odd+1,Odd_Rows+1,Even_Rows)   + s(Black_Odd+1,Odd_Rows-1,Even_Rows)   + 
               s(Black_Odd-1,Odd_Rows+1,Even_Rows)   + s(Black_Odd-1,Odd_Rows-1,Even_Rows)   +
               s(Black_Odd+1,Odd_Rows,Even_Rows+1)   + s(Black_Odd-1,Odd_Rows,Even_Rows+1)   + 
               s(Black_Odd,Odd_Rows+1,Even_Rows+1)   + s(Black_Odd,Odd_Rows-1,Even_Rows+1)   + 
               s(Black_Odd+1,Odd_Rows+1,Even_Rows+1) + s(Black_Odd+1,Odd_Rows-1,Even_Rows+1) + 
               s(Black_Odd-1,Odd_Rows+1,Even_Rows+1) + s(Black_Odd-1,Odd_Rows-1,Even_Rows+1) +
               s(Black_Odd,Odd_Rows,Even_Rows-1)     + s(Black_Odd,Odd_Rows,Even_Rows+1) ) / 26.0;
     s(Black_Even,Even_Rows,Even_Rows) = 
          ( hSqr * rhs(Black_Even,Even_Rows,Even_Rows) + 
               s(Black_Even+1,Even_Rows,Even_Rows-1)   + s(Black_Even-1,Even_Rows,Even_Rows-1)   + 
               s(Black_Even,Even_Rows+1,Even_Rows-1)   + s(Black_Even,Even_Rows-1,Even_Rows-1)   + 
               s(Black_Even+1,Even_Rows+1,Even_Rows-1) + s(Black_Even+1,Even_Rows-1,Even_Rows-1) + 
               s(Black_Even-1,Even_Rows+1,Even_Rows-1) + s(Black_Even-1,Even_Rows-1,Even_Rows-1) +
               s(Black_Even+1,Even_Rows,Even_Rows)     + s(Black_Even-1,Even_Rows,Even_Rows)     + 
               s(Black_Even,Even_Rows+1,Even_Rows)     + s(Black_Even,Even_Rows-1,Even_Rows)     + 
               s(Black_Even+1,Even_Rows+1,Even_Rows)   + s(Black_Even+1,Even_Rows-1,Even_Rows)   + 
               s(Black_Even-1,Even_Rows+1,Even_Rows)   + s(Black_Even-1,Even_Rows-1,Even_Rows)   +
               s(Black_Even+1,Even_Rows,Even_Rows+1)   + s(Black_Even-1,Even_Rows,Even_Rows+1)   + 
               s(Black_Even,Even_Rows+1,Even_Rows+1)   + s(Black_Even,Even_Rows-1,Even_Rows+1)   + 
               s(Black_Even+1,Even_Rows+1,Even_Rows+1) + s(Black_Even+1,Even_Rows-1,Even_Rows+1) + 
               s(Black_Even-1,Even_Rows+1,Even_Rows+1) + s(Black_Even-1,Even_Rows-1,Even_Rows+1) +
               s(Black_Even,Even_Rows,Even_Rows-1)     + s(Black_Even,Even_Rows,Even_Rows+1) ) / 26.0;
     s(Red_Even,Odd_Rows,Odd_Rows) = 
          ( hSqr * rhs(Red_Even,Odd_Rows,Odd_Rows) + 
               s(Red_Even+1,Odd_Rows,Odd_Rows-1)   + s(Red_Even-1,Odd_Rows,Odd_Rows-1)   + 
               s(Red_Even,Odd_Rows+1,Odd_Rows-1)   + s(Red_Even,Odd_Rows-1,Odd_Rows-1)   + 
               s(Red_Even+1,Odd_Rows+1,Odd_Rows-1) + s(Red_Even+1,Odd_Rows-1,Odd_Rows-1) + 
               s(Red_Even-1,Odd_Rows+1,Odd_Rows-1) + s(Red_Even-1,Odd_Rows-1,Odd_Rows-1) +
               s(Red_Even+1,Odd_Rows,Odd_Rows)     + s(Red_Even-1,Odd_Rows,Odd_Rows)     + 
               s(Red_Even,Odd_Rows+1,Odd_Rows)     + s(Red_Even,Odd_Rows-1,Odd_Rows)     + 
               s(Red_Even+1,Odd_Rows+1,Odd_Rows)   + s(Red_Even+1,Odd_Rows-1,Odd_Rows)   + 
               s(Red_Even-1,Odd_Rows+1,Odd_Rows)   + s(Red_Even-1,Odd_Rows-1,Odd_Rows)   +
               s(Red_Even+1,Odd_Rows,Odd_Rows+1)   + s(Red_Even-1,Odd_Rows,Odd_Rows+1)   + 
               s(Red_Even,Odd_Rows+1,Odd_Rows+1)   + s(Red_Even,Odd_Rows-1,Odd_Rows+1)   + 
               s(Red_Even+1,Odd_Rows+1,Odd_Rows+1) + s(Red_Even+1,Odd_Rows-1,Odd_Rows+1) + 
               s(Red_Even-1,Odd_Rows+1,Odd_Rows+1) + s(Red_Even-1,Odd_Rows-1,Odd_Rows+1) +
               s(Red_Even,Odd_Rows,Odd_Rows-1)     + s(Red_Even,Odd_Rows,Odd_Rows+1) ) / 26.0;
     s(Red_Odd,Even_Rows,Odd_Rows) = 
          ( hSqr * rhs(Red_Odd,Even_Rows,Odd_Rows) + 
               s(Red_Odd+1,Even_Rows,Odd_Rows-1)   + s(Red_Odd-1,Even_Rows,Odd_Rows-1)   + 
               s(Red_Odd,Even_Rows+1,Odd_Rows-1)   + s(Red_Odd,Even_Rows-1,Odd_Rows-1)   + 
               s(Red_Odd+1,Even_Rows+1,Odd_Rows-1) + s(Red_Odd+1,Even_Rows-1,Odd_Rows-1) + 
               s(Red_Odd-1,Even_Rows+1,Odd_Rows-1) + s(Red_Odd-1,Even_Rows-1,Odd_Rows-1) +
               s(Red_Odd+1,Even_Rows,Odd_Rows)     + s(Red_Odd-1,Even_Rows,Odd_Rows)     + 
               s(Red_Odd,Even_Rows+1,Odd_Rows)     + s(Red_Odd,Even_Rows-1,Odd_Rows)     + 
               s(Red_Odd+1,Even_Rows+1,Odd_Rows)   + s(Red_Odd+1,Even_Rows-1,Odd_Rows)   + 
               s(Red_Odd-1,Even_Rows+1,Odd_Rows)   + s(Red_Odd-1,Even_Rows-1,Odd_Rows)   +
               s(Red_Odd+1,Even_Rows,Odd_Rows+1)   + s(Red_Odd-1,Even_Rows,Odd_Rows+1)   + 
               s(Red_Odd,Even_Rows+1,Odd_Rows+1)   + s(Red_Odd,Even_Rows-1,Odd_Rows+1)   + 
               s(Red_Odd+1,Even_Rows+1,Odd_Rows+1) + s(Red_Odd+1,Even_Rows-1,Odd_Rows+1) + 
               s(Red_Odd-1,Even_Rows+1,Odd_Rows+1) + s(Red_Odd-1,Even_Rows-1,Odd_Rows+1) +
               s(Red_Odd,Even_Rows,Odd_Rows-1)     + s(Red_Odd,Even_Rows,Odd_Rows+1) ) / 26.0;
     s(Red_Even,Odd_Rows,Even_Rows) = 
          ( hSqr * rhs(Red_Even,Odd_Rows,Even_Rows) + 
               s(Red_Even+1,Odd_Rows,Even_Rows-1)   + s(Red_Even-1,Odd_Rows,Even_Rows-1)   + 
               s(Red_Even,Odd_Rows+1,Even_Rows-1)   + s(Red_Even,Odd_Rows-1,Even_Rows-1)   + 
               s(Red_Even+1,Odd_Rows+1,Even_Rows-1) + s(Red_Even+1,Odd_Rows-1,Even_Rows-1) + 
               s(Red_Even-1,Odd_Rows+1,Even_Rows-1) + s(Red_Even-1,Odd_Rows-1,Even_Rows-1) +
               s(Red_Even+1,Odd_Rows,Even_Rows)     + s(Red_Even-1,Odd_Rows,Even_Rows)     + 
               s(Red_Even,Odd_Rows+1,Even_Rows)     + s(Red_Even,Odd_Rows-1,Even_Rows)     + 
               s(Red_Even+1,Odd_Rows+1,Even_Rows)   + s(Red_Even+1,Odd_Rows-1,Even_Rows)   + 
               s(Red_Even-1,Odd_Rows+1,Even_Rows)   + s(Red_Even-1,Odd_Rows-1,Even_Rows)   +
               s(Red_Even+1,Odd_Rows,Even_Rows+1)   + s(Red_Even-1,Odd_Rows,Even_Rows+1)   + 
               s(Red_Even,Odd_Rows+1,Even_Rows+1)   + s(Red_Even,Odd_Rows-1,Even_Rows+1)   + 
               s(Red_Even+1,Odd_Rows+1,Even_Rows+1) + s(Red_Even+1,Odd_Rows-1,Even_Rows+1) + 
               s(Red_Even-1,Odd_Rows+1,Even_Rows+1) + s(Red_Even-1,Odd_Rows-1,Even_Rows+1) +
               s(Red_Even,Odd_Rows,Even_Rows-1)     + s(Red_Even,Odd_Rows,Even_Rows+1) ) / 26.0;
     s(Red_Odd,Even_Rows,Even_Rows) = 
          ( hSqr * rhs(Red_Odd,Even_Rows,Even_Rows) + 
               s(Red_Odd+1,Even_Rows,Even_Rows-1)   + s(Red_Odd-1,Even_Rows,Even_Rows-1)   + 
               s(Red_Odd,Even_Rows+1,Even_Rows-1)   + s(Red_Odd,Even_Rows-1,Even_Rows-1)   + 
               s(Red_Odd+1,Even_Rows+1,Even_Rows-1) + s(Red_Odd+1,Even_Rows-1,Even_Rows-1) + 
               s(Red_Odd-1,Even_Rows+1,Even_Rows-1) + s(Red_Odd-1,Even_Rows-1,Even_Rows-1) +
               s(Red_Odd+1,Even_Rows,Even_Rows)     + s(Red_Odd-1,Even_Rows,Even_Rows)     + 
               s(Red_Odd,Even_Rows+1,Even_Rows)     + s(Red_Odd,Even_Rows-1,Even_Rows)     + 
               s(Red_Odd+1,Even_Rows+1,Even_Rows)   + s(Red_Odd+1,Even_Rows-1,Even_Rows)   + 
               s(Red_Odd-1,Even_Rows+1,Even_Rows)   + s(Red_Odd-1,Even_Rows-1,Even_Rows)   +
               s(Red_Odd+1,Even_Rows,Even_Rows+1)   + s(Red_Odd-1,Even_Rows,Even_Rows+1)   + 
               s(Red_Odd,Even_Rows+1,Even_Rows+1)   + s(Red_Odd,Even_Rows-1,Even_Rows+1)   + 
               s(Red_Odd+1,Even_Rows+1,Even_Rows+1) + s(Red_Odd+1,Even_Rows-1,Even_Rows+1) + 
               s(Red_Odd-1,Even_Rows+1,Even_Rows+1) + s(Red_Odd-1,Even_Rows-1,Even_Rows+1) +
               s(Red_Odd,Even_Rows,Even_Rows-1)     + s(Red_Odd,Even_Rows,Even_Rows+1) ) / 26.0;
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

