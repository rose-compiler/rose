#include "simpleA++.h"

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
   }
