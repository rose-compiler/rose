// Need to build a Red-Black relaxation example and an Interpolation example for Qing.
// Look at operations on arrays with multiple components (that translate into strided 
// operations), as well.  These examples plus the riemman.C code will for examples for 
// a paper on loop transformations and there role in the specification of high-level 
// abstractions.




void PDE::Red_Black_Relax ()
   {
  // This function implements red/black relaxation
  // on the interior of the array and is further restricted
  // to use with odd numbered dimension lengths.

  // Warning: For odd-numbered dimensinons only!
  //          Relaxation on INTERIOR only!

     int Alt_Base = I.getBase()-1;

  // APP_DEBUG = 5;
  // printf ("Inside of PDE::Red_Black_Relax -- Alt_Base = %d \n",Alt_Base);

     Index Black_Odd  ( Alt_Base+1 , (Grid_Size - 1) / 2 , 2 );
     Index Black_Even ( Alt_Base+2 , (Grid_Size - 2) / 2 , 2 );

     Index Red_Odd   ( Alt_Base+1 , (Grid_Size - 1) / 2 , 2 );
     Index Red_Even  ( Alt_Base+2 , (Grid_Size - 2) / 2 , 2 );

     Index Odd_Rows  ( Alt_Base+1 , (Grid_Size - 1) / 2 , 2 );
     Index Even_Rows ( Alt_Base+2 , (Grid_Size - 2) / 2 , 2 );

     if (Problem_Dimension == 1)
        {
          Three_Point_Stencil ( Black_Even );
          Three_Point_Stencil ( Black_Odd  );
        }
       else
        {
          if (Problem_Dimension == 2)
             {
               if (Fully_Variational)
                  {
                    Nine_Point_Stencil ( Black_Odd  , Odd_Rows  );
                    Nine_Point_Stencil ( Black_Even , Even_Rows );
                    Nine_Point_Stencil ( Red_Even   , Odd_Rows  );
                    Nine_Point_Stencil ( Red_Odd    , Even_Rows );
                  }
                 else 
                  {
                    Five_Point_Stencil ( Black_Odd  , Odd_Rows  );
                    Five_Point_Stencil ( Black_Even , Even_Rows );
                    Five_Point_Stencil ( Red_Even   , Odd_Rows  );
                    Five_Point_Stencil ( Red_Odd    , Even_Rows );
                  }
             }
            else
             {
               if (Problem_Dimension == 3)
                  {
                    if (Fully_Variational)
                       {
                         Twenty_Seven_Point_Stencil ( Black_Odd  , Odd_Rows  , Odd_Rows  );
                         Twenty_Seven_Point_Stencil ( Black_Even , Even_Rows , Odd_Rows  );
                         Twenty_Seven_Point_Stencil ( Black_Odd  , Odd_Rows  , Even_Rows );
                         Twenty_Seven_Point_Stencil ( Black_Even , Even_Rows , Even_Rows );

                         Twenty_Seven_Point_Stencil ( Red_Even   , Odd_Rows  , Odd_Rows  );
                         Twenty_Seven_Point_Stencil ( Red_Odd    , Even_Rows , Odd_Rows  );
                         Twenty_Seven_Point_Stencil ( Red_Even   , Odd_Rows  , Even_Rows );
                         Twenty_Seven_Point_Stencil ( Red_Odd    , Even_Rows , Even_Rows );
                       }
                      else
                       {
                         Seven_Point_Stencil ( Black_Odd  , Odd_Rows  , Odd_Rows  );
                         Seven_Point_Stencil ( Black_Even , Even_Rows , Odd_Rows  );
                         Seven_Point_Stencil ( Black_Odd  , Odd_Rows  , Even_Rows );
                         Seven_Point_Stencil ( Black_Even , Even_Rows , Even_Rows );

                         Seven_Point_Stencil ( Red_Even   , Odd_Rows  , Odd_Rows  );
                         Seven_Point_Stencil ( Red_Odd    , Even_Rows , Odd_Rows  );
                         Seven_Point_Stencil ( Red_Even   , Odd_Rows  , Even_Rows );
                         Seven_Point_Stencil ( Red_Odd    , Even_Rows , Even_Rows );
                       }
                  }
             }
        }
   }

