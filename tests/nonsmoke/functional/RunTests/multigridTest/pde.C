//              ***********  C++ PDE CODE  ************

#include "mg_main.h"

#define DEBUG 0

#ifdef GRAPHICS
extern GraphicsWindow Graphics_Object;
#endif

// ****************************************************************************
//                  PDE (Uniform Square Grid) C++ CLASS
// ****************************************************************************

// Static class data members must be declared!
int PDE::Problem_Dimension = 2;
Boolean PDE::Fully_Variational = TRUE;


PDE::PDE ()
   {
  // Nothing to do here since the default constructors
  // for all our data objects are called automatically!

  // Provide some default values for scalar data fields!
     Mesh_Size = 1.0;
     Grid_Size = 1;
   }

PDE::~PDE ()
   {
  // A very simple destructor!
  // Nothing to delete since we have no pointers to other objects!
   }

PDE::PDE ( int Local_Grid_Size )
   {
  // This is a very simple constructor, it allows the creation of an object
  // with arrays of a specific size (then the Rhs can be setup seperately).

     Construct_PDE  ( Local_Grid_Size );
   }

PDE::PDE ( int Local_Grid_Size ,
           double (* Function_RHS)            ( double X , double Y , double Z ) ,
           double (* Exact_Solution_Function) ( double X , double Y , double Z ) )
   {
  // This constructor for the PDE object assumes the existence of an exact solution and
  // allows it to be passed into the object so that error norms will be meaning full.

     printf ("Call: Construct_PDE  ( Local_Grid_Size = %d ) \n",Local_Grid_Size);
     Construct_PDE  ( Local_Grid_Size );
     printf ("CALL: Initialize_PDE ( Function_RHS ) \n");
     Initialize_PDE ( Function_RHS );
     Setup_PDE_Exact_Solution ( Exact_Solution_Function );

#if 1
  // Setup RHS so we can compute the exact solution with zero discretization error!
     Solution = Exact_Solution;
     Right_Hand_Side = 0.0;
     printf ("CALL: Compute_Residuals () \n");
     Compute_Residuals();
  // Temp code!
  // Residual = 0.0;

  // APP_DEBUG = 5;
  // Right_Hand_Side.view("Right_Hand_Side");
  // Residual.view("Residual");
     Right_Hand_Side = -Residual;
#endif

     Residual = 0.0;
     Solution = 0.0;

#if 0
     Residual.display("Residual");
     Right_Hand_Side.display("Right_Hand_Side");
     Solution.display("Solution");
     Exact_Solution.display("Exact_Solution");
#endif

  // printf ("Exiting in PDE::PDE! \n");
  // APP_ABORT();
   }

void PDE::Setup_PDE_Exact_Solution ( double (* Exact_Solution_Function) ( double X , double Y , double Z ) )
   {
  // Setup the Exact Solution with the values from the function defineing the exact solution!

     Initialize_Array_To_Function ( Exact_Solution , Exact_Solution_Function );
   }

void PDE::Setup_RHS ()
   {
     const double PI = 3.14159265;

  // If we compute these once here, then we can avoid 
  // redundent function call overhead inside the loop structure
     int Base_K = Right_Hand_Side.getBase(2);
     int Base_J = Right_Hand_Side.getBase(1);
     int Base_I = Right_Hand_Side.getBase(0);

     int Bound_K = Right_Hand_Side.getBound(2);
     int Bound_J = Right_Hand_Side.getBound(1);
     int Bound_I = Right_Hand_Side.getBound(0);

     if (PDE::Problem_Dimension == 1)
        {
       // return sin (X*PI) * (PI * PI);
          doubleArray Temp_X(Right_Hand_Side.getLength(0));
          Temp_X.seqAdd(0);
          Right_Hand_Side = sin(Temp_X*(Mesh_Size*PI)) * (PI * PI); 
        }
       else
        {
          if (PDE::Problem_Dimension == 2)
             {
               doubleArray Temp_X (Right_Hand_Side.getLength(0));
               doubleArray Temp_Y (Right_Hand_Side.getLength(0));
               Temp_X.seqAdd(0);
               Temp_Y.seqAdd(0);
            // Temp_X.display("Temp_X");
            // Temp_Y.display("Temp_Y");
               if (PDE::Fully_Variational)
                  {
                 // return ( sin (X*PI) + sin(Y*PI) ) * (PI * PI) * 3.0;
                 // Right_Hand_Side = sin(Temp_X*(Mesh_Size*PI)) * sin(Temp_Y*(Mesh_Size*PI)) * (PI * PI) * 3.0; 
                    Temp_X = sin(Temp_X * (PI * Mesh_Size)) * 3.0;
                    Temp_Y = sin(Temp_Y * (PI * Mesh_Size)) * (PI * PI);
                  }
                 else
                  {
                 // return ( sin (X*PI) + sin(Y*PI) ) * (PI * PI);
                 // Right_Hand_Side = sin(Temp_X*PI) * sin(Temp_Y*PI) * (PI * PI); 
                    Temp_X = sin(Temp_X * (PI * Mesh_Size));
                    Temp_Y = sin(Temp_Y * (PI * Mesh_Size)) * (PI * PI); 
                  }

            // Temp_X.display("Temp_X");
            // Temp_Y.display("Temp_Y");
               Range range_I (Right_Hand_Side.getBase(0),Right_Hand_Side.getBound(0));

            // printf ("Now do the assignment to the Right_Hand_Side \n");
               for (int j = Temp_Y.getBase(0); j <= Temp_Y.getBound(0); j++)
                  { 
                 // double yy = Temp_Y(j);
                    double yy = sin(j * (PI * Mesh_Size)) * (PI * PI);
                 // printf ("yy = %f \n",yy);
                 // Right_Hand_Side (range_I,j+Base_J).display("Right_Hand_Side (range_I,j+Base_J)");
                 // Temp_X.display("Temp_X");
                    
                    Right_Hand_Side (range_I,j+Base_J) = Temp_X * yy;
                  }
            // Right_Hand_Side.display("Right_Hand_Side");

            // printf ("Exiting in PDE::Setup_RHS \n");
            // APP_ABORT();
             }
            else
             {
               doubleArray Temp_X (Right_Hand_Side.getLength(0));
               doubleArray Temp_Y (Right_Hand_Side.getLength(0));
               doubleArray Temp_Z (Right_Hand_Side.getLength(0));
               Temp_X.seqAdd(0);
               Temp_Y.seqAdd(0);
               Temp_Z.seqAdd(0);
               if (PDE::Fully_Variational)
                  {
                 // return ( sin (X*PI) + sin(Y*PI) + sin (Z*PI) ) * (PI * PI) * 6.0;
                 // Right_Hand_Side = sin(Temp_X*PI) * sin(Temp_Y*PI) * sin(Temp_Z*PI) * (PI * PI) * 6.0;
                    Temp_X = sin(Temp_X*PI);
                    Temp_Y = sin(Temp_Y*PI);
                    Temp_Z = sin(Temp_Z*PI) * (PI * PI * 6.0);
                  }
                 else
                  {
                 // return ( sin (X*PI) + sin(Y*PI) + sin (Z*PI) ) * (PI * PI);
                 // Right_Hand_Side = sin(Temp_X*PI) * sin(Temp_Y*PI) * sin(Temp_Z*PI) * (PI * PI); 
                    Temp_X = sin(Temp_X*PI);
                    Temp_Y = sin(Temp_Y*PI);
                    Temp_Z = sin(Temp_Z*PI) * (PI * PI);
                  }

               for (int k = Base_K; k <= Bound_K; k++)
                  {
                    for (int j = Base_J; j <= Bound_J; j++)
                       { 
                         for (int i = Base_I; i <= Bound_I; i++)
                            { 
                              Right_Hand_Side (i,j,k) = Temp_X(i-Base_I) * Temp_Y(j-Base_J) * Temp_Z(k-Base_K);
                            }
                       }
                  }
             }
        }
   }

void PDE::Initialize_Array_To_Function ( doubleArray & Array , 
                                         double (* Primative_Function) ( double X , double Y , double Z ) )
   {
  // This function initializes the input array using the input function.  Notice that
  // it is independent of any particular base and bound of the array, it queries the
  // array object for its base and bound (stride is assumed to be 1 in this function).
  // This sort of scalar indexing is slower than normal FORTRAN of C, but this setup is
  // not a significant part of the solver so it makes little difference.  Scalar indexing
  // is inlined and because of this can be as efficient as FORTRAN indexing with some C++
  // compilers. This could be implemented differently to avoidthe function call and in doing so
  // the inlined code should be as efficient as FORTRAN.

     double X,Y,Z = 0;

     if (DEBUG > 1)
          printf ("Inside of PDE::Initialize_Array_To_Function \n");
#if 1
  // This used P++ functionality to just operate upon the local pieces of the distributed array objects
  // This will also execute in A++.

  // If we compute these once here, then we can avoid 
  // redundent function call overhead inside the loop structure
     int Base_K = Array.getBase(2);
     int Base_J = Array.getBase(1);
     int Base_I = Array.getBase(0);

     int LocalBase_K = Array.getLocalBase(2);
     int LocalBase_J = Array.getLocalBase(1);
     int LocalBase_I = Array.getLocalBase(0);

     int Bound_K = Array.getLocalBound(2);
     int Bound_J = Array.getLocalBound(1);
     int Bound_I = Array.getLocalBound(0);

     Optimization_Manager::setOptimizedScalarIndexing(On);
     for (int k = LocalBase_K; k <= Bound_K; k++)
        {
          for (int j = LocalBase_J; j <= Bound_J; j++)
             { 
               for (int i = LocalBase_I; i <= Bound_I; i++)
                  { 
                    X = Mesh_Size * double (i-Base_I);
                    Y = Mesh_Size * double (j-Base_J);
                    Z = Mesh_Size * double (k-Base_K);
                 // printf ("Before: X=%f   Y=%f   Z=%f \n",X,Y,Z);
                    Array (i,j,k) = (* Primative_Function) ( X , Y , Z );
                 // printf ("After: X=%f   Y=%f   Z=%f  Array(%d,%d,%d) = %f \n",X,Y,Z,i,j,k,Array(i,j,k));
                  }
             }
        }
     Optimization_Manager::setOptimizedScalarIndexing(Off);
#else
  // If we compute these once here, then we can avoid 
  // redundent function call overhead inside the loop structure
     int Base_K = Array.getBase(2);
     int Base_J = Array.getBase(1);
     int Base_I = Array.getBase(0);
  // int Base_J = Base_K;
  // int Base_I = Base_K;

     int Bound_K = Array.getBound(2);
     int Bound_J = Array.getBound(1);
     int Bound_I = Array.getBound(0);

  // printf ("Base_I  = %d  Base_J  = %d  Base_K  = %d \n",Base_I,Base_J,Base_K);
  // printf ("Bound_I = %d  Bound_J = %d  Bound_K = %d \n",Bound_I,Bound_J,Bound_K);

     Optimization_Manager::setOptimizedScalarIndexing(On);
     for (int k = Base_K; k <= Bound_K; k++)
        {
          for (int j = Base_J; j <= Bound_J; j++)
             { 
               for (int i = Base_I; i <= Bound_I; i++)
                  { 
                    X = Mesh_Size * double (i-Base_I);
                    Y = Mesh_Size * double (j-Base_J);
                    Z = Mesh_Size * double (k-Base_K);
                 // printf ("Before: X=%f   Y=%f   Z=%f \n",X,Y,Z);
                    Array (i,j,k) = (* Primative_Function) ( X , Y , Z );
                 // printf ("After: X=%f   Y=%f   Z=%f  Array(%d,%d,%d) = %f \n",X,Y,Z,i,j,k,Array(i,j,k));
                  }
             }
        }

     Optimization_Manager::setOptimizedScalarIndexing(Off);
#endif

  // Array.getSerialArrayPointer()->display("Local Array Data");
   }

void PDE::Construct_PDE ( int Local_Grid_Size )
   {
     if (DEBUG > 0)
          printf ("Inside of PDE::Construct_PDE \n");

     printf ("Local_Grid_Size = %d \n",Local_Grid_Size);

  // This variable may be set to any value, it defines the base of the 
  // arrays that are construccted in this object.  It was uses as a test to 
  // debug the ability in A++ to has arrays with any base (positive or negative).
  // Range is -2,147,483,647 to 2,147,483,647-arraysize!
  // int Alt_Base = 1000000000;
  // int Alt_Base = 0;
  // int Alt_Base = 1;
     int Alt_Base = -1000;

  // Initialize objects scalar fields
     Grid_Size = Local_Grid_Size;
     Mesh_Size = 1.0 / double (Local_Grid_Size - 1);

  // Since the default constructor already built the PDE objects it already called
  // the default constructors for the array objects!  So we have to redimension the existing
  // array objects!

  // APP_DEBUG = 5;
     if (Problem_Dimension == 1)
        {
          I = Index ( 1+Alt_Base , Grid_Size - 2 , 1 );
       // printf ("Turn ON APP_DEBUG = 5 in PDE::Construct_PDE \n");
       // APP_DEBUG = 6;
          Solution.redim        ( Grid_Size );
       // APP_DEBUG = 0;

       // printf ("Exiting in PDE::Construct_PDE ... \n");
       // APP_ABORT();

          Right_Hand_Side.redim ( Grid_Size );
          Residual.redim        ( Grid_Size );
          Exact_Solution.redim  ( Grid_Size );
        }
       else
        {
          if (Problem_Dimension == 2)
             {
               I = Index ( 1+Alt_Base , Grid_Size - 2 , 1 );
               J = I;
               Solution.redim        ( Grid_Size , Grid_Size );
               Right_Hand_Side.redim ( Grid_Size , Grid_Size );
               Residual.redim        ( Grid_Size , Grid_Size );
               Exact_Solution.redim  ( Grid_Size , Grid_Size );
             }
            else
             {
               if (Problem_Dimension == 3)
                  {
                    I = Index ( 1+Alt_Base , Grid_Size - 2 , 1 );
                    J = I;
                    K = I;
                    Solution.redim        ( Grid_Size , Grid_Size , Grid_Size );
                    Right_Hand_Side.redim ( Grid_Size , Grid_Size , Grid_Size );
                    Residual.redim        ( Grid_Size , Grid_Size , Grid_Size );
                    Exact_Solution.redim  ( Grid_Size , Grid_Size , Grid_Size );
                  }
                 else
                  {
                    printf ("ERROR: Problem_Dimension > 3! \n");
                    exit (1);
                  }
             }
        }

     printf ("Call setBase! \n");

     Solution.setBase        ( Alt_Base );
     Right_Hand_Side.setBase ( Alt_Base );
     Residual.setBase        ( Alt_Base );
     Exact_Solution.setBase  ( Alt_Base );

  // printf ("Set Solution(I) = 0 \n");
  // I.display("I");
  // Solution.view("Solution");
  // Solution(I).view("Solution(I)");
  // Solution(I) = 0;

  // printf ("Exiting in PDE::Construct_PDE ... \n");
  // APP_ABORT();

     if (DEBUG > 0)
          printf ("Leaving PDE::Construct_PDE \n");
   }

void PDE::Initialize_PDE ( double (* Function_RHS) ( double X , double Y , double Z ) )
   {
     printf ("TOP of Initialize_PDE! \n");
     Initialize_Array_To_Function ( Right_Hand_Side , Function_RHS );

  // Update the ghost boundaries to make them consistant and also
  // avoid the purify UMR warning which was forcing a read of the 
  // uninitialized ghost boundaries
  // This is now done in the P++ redim function!
     Solution.updateGhostBoundaries();
  // Right_Hand_Side.updateGhostBoundaries();
  // Residual.updateGhostBoundaries();
  // Exact_Solution.updateGhostBoundaries();

  // Now scale the Right_Hand_Side!
  // Right_Hand_Side *= (Mesh_Size * Mesh_Size);
     printf ("BOTTOM of Initialize_PDE! \n");
   }

void PDE::Jacobi_Relax ()
   {
  // This function implements Jacobi relaxation.
  // This is the simple case of using the existing I,J index objects for the
  // interior of the grid and handing them to the Five_Point_Stencil member function.

     if (Problem_Dimension == 1)
        {
          Three_Point_Stencil ( I );
        }
       else
        {
          if (Problem_Dimension == 2)
             {
               if (Fully_Variational)
                  {
                    Nine_Point_Stencil ( I , J );
                  }
                 else
                  {
                    Five_Point_Stencil ( I , J );
                  }
             }
            else
             {
               if (Problem_Dimension == 3)
                  {
                    if (Fully_Variational)
                       {
                         Twenty_Seven_Point_Stencil ( I , J , K );
                       }
                      else
                       {
                         Seven_Point_Stencil ( I , J , K );
                       }
                  }
             }
        }
   }

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

void PDE::Solve ()
   { 
  // This is the Solve function for this object (every object pretending to represent a solver 
  // should have one (the MG objects solver calls this one).  Thus details of the 
  // descritization of the MG problem are isolated (and the complexities contained) in the PDE
  // object where the specific application problem (together with the descritization of it)
  // is hidden.  In this way the MG solve is relatively independent of the specifics of the
  // particular application.

  // We choose to use the Red_Black relaxation here!
  // Red_Black_Relax ();

     if (DEBUG > 0)
        {
          Right_Hand_Side.display("RHS -- BEFORE RELAX");
          Solution.display("SOLUTION -- BEFORE RELAX");
        }

  // Jacobi_Relax ();
     Red_Black_Relax ();

#ifdef GRAPHICS
     printf ("START: Contour Solution \n");
     Graphics_Object.setProperty(TOPLABEL,"Solution");
     Graphics_Object.setProperty(CONTSTYLE,2);
     Graphics_Object.setProperty(NSTEPS,10);
     Graphics_Object.contour(Solution);
     printf ("END: Contour Solution \n");
#endif

     if (DEBUG > 0)
          Solution.display("SOLUTION -- AFTER RELAX");
   }

void PDE::Error ()
   { 
  // This error function computes the error and then prints a few norms of it!

     if (DEBUG > 0)
          printf ("Inside of PDE::Error() \n");

     doubleArray Error = fabs (Solution (I,J,K) - Exact_Solution (I,J,K));
  // printf ("L2_Norm Error = %e Max Error = %e Min Error = %e \n", L2_Norm (Error) , max(Error) , min(Error) );
   }

void PDE::Compute_Residuals ()
   {
  // The decision to implement the residual computation seperately is a design preference.

     if (DEBUG > 0)
          printf ("Compute Residuals! \n");

     Residual = 0.0;

     double Scaling_Factor = 1.0 / (Mesh_Size * Mesh_Size);
     if (Problem_Dimension == 1)
        {
          Residual (I) = Right_Hand_Side (I) + Scaling_Factor * ( 
                         Solution (I+1) + Solution (I-1) - 2.0 * Solution (I) );
        }
       else
        {
          if (Problem_Dimension == 2)
             {
               if (Fully_Variational)
                  {
                    Residual (I,J) = Right_Hand_Side (I,J) + Scaling_Factor * ( 
                                     Solution (I+1,J)   + Solution (I-1,J)   + Solution (I,J-1)   + Solution (I,J+1) +
                                     Solution (I+1,J+1) + Solution (I-1,J+1) + Solution (I+1,J-1) + Solution (I-1,J-1) - 
                                     8.0 * Solution (I,J) );
                  }
                 else
                  {
                    Residual (I,J) = Right_Hand_Side (I,J) + Scaling_Factor * ( 
                                     Solution (I+1,J) + Solution (I-1,J) + 
                                     Solution (I,J-1) + Solution (I,J+1) - 4.0 * Solution (I,J) );
                  }
#ifdef GRAPHICS
               Graphics_Object.setProperty(TOPLABEL,"Residual");
               Graphics_Object.setProperty(CONTSTYLE,2);
               Graphics_Object.setProperty(NSTEPS,10);
               Graphics_Object.contour(Residual);
#endif

             }
            else
             {
               if (Problem_Dimension == 3)
                  {
                    if (Fully_Variational)
                       {
                         Residual (I,J,K) = Right_Hand_Side (I,J,K) + Scaling_Factor * ( 
                                         // Center face
                                            Solution (I+1,J,K)   + Solution (I-1,J,K)       +
                                            Solution (I,J-1,K)   + Solution (I,J+1,K)       +
                                            Solution (I+1,J+1,K) + Solution (I-1,J+1,K)     +
                                            Solution (I+1,J-1,K) + Solution (I-1,J-1,K)     +
                                         // Inner face
                                            Solution (I+1,J,K-1)   + Solution (I-1,J,K-1)   +
                                            Solution (I,J-1,K-1)   + Solution (I,J+1,K-1)   +
                                            Solution (I+1,J+1,K-1) + Solution (I-1,J+1,K-1) +
                                            Solution (I+1,J-1,K-1) + Solution (I-1,J-1,K-1) +
                                         // Outer face
                                            Solution (I+1,J,K+1)   + Solution (I-1,J,K+1)   +
                                            Solution (I,J-1,K+1)   + Solution (I,J+1,K+1)   +
                                            Solution (I+1,J+1,K+1) + Solution (I-1,J+1,K+1) +
                                            Solution (I+1,J-1,K+1) + Solution (I-1,J-1,K+1) +
                                         // Center point of Inner and Outer faces
                                            Solution (I,J,K-1) + Solution (I,J,K+1) -
                                          // Center point
                                            26.0 * Solution (I,J,K) );
                       }
                      else
                       {
                         Residual (I,J,K) = Right_Hand_Side (I,J,K) + Scaling_Factor * (
                                            Solution (I+1,J,K) + Solution (I-1,J,K) + 
                                            Solution (I,J-1,K) + Solution (I,J+1,K) + Solution (I,J,K-1) + 
                                            Solution (I,J,K+1) - 6.0 * Solution (I,J,K) );
                       }
                  }
             }
        }

     if (DEBUG > 0)
          printf ("Leaving Compute Residuals! \n");

  // Swap sign for test!
  // Residual = -Residual; 
   }

double PDE::L2_Norm ( doubleArray & Array )
   {
  // This is the most natural AND most efficient implementation using the array class!

     if (DEBUG > 0)
          printf ("Inside of PDE::L2_Norm! \n");

     return sqrt ( sum ( Array * Array ) );
   }

void PDE::Three_Point_Stencil ( Index & i )
   {
     if (DEBUG > 0)
          printf ("Compute Three_Point_Stencil! \n");

     Solution (i) = ( (Mesh_Size * Mesh_Size) * Right_Hand_Side (i) + Solution (i+1) + Solution (i-1) ) / 2.0;

     if (DEBUG > 0)
          printf ("Leaving Three_Point_Stencil! \n");
   }

void PDE::Five_Point_Stencil ( Index & i , Index & j )
   {
     Solution (i,j) = ( (Mesh_Size * Mesh_Size) * Right_Hand_Side (i,j) + Solution (i+1,j) + 
                        Solution (i-1,j) + Solution (i,j+1) + Solution (i,j-1) ) / 4.0;
   }

void PDE::Nine_Point_Stencil ( Index & i , Index & j )
   {
     Solution (i,j) = ( (Mesh_Size * Mesh_Size) * Right_Hand_Side (i,j) + 
                        Solution (i+1,j) + Solution (i-1,j) + Solution (i,j+1) + Solution (i,j-1) + 
                        Solution (i+1,j+1) + Solution (i+1,j-1) + Solution (i-1,j+1) + Solution (i-1,j-1)) / 8.0;
   }

void PDE::Seven_Point_Stencil ( Index & i , Index & j , Index & k )
   {
     Solution (i,j,k) = ( (Mesh_Size * Mesh_Size) * Right_Hand_Side (i,j,k) + 
                        Solution (i+1,j,k) + Solution (i-1,j,k) + Solution (i,j+1,k) + 
                        Solution (i,j-1,k) + Solution (i,j,k+1) + Solution (i,j,k-1) ) / 6.0;
   }

void PDE::Twenty_Seven_Point_Stencil ( Index & i , Index & j , Index & k )
   {
     Solution (i,j,k) = ( (Mesh_Size * Mesh_Size) * Right_Hand_Side (i,j,k) + 
              Solution (i+1,j,k-1)   + Solution (i-1,j,k-1)   + Solution (i,j+1,k-1)   + Solution (i,j-1,k-1)   + 
              Solution (i+1,j+1,k-1) + Solution (i+1,j-1,k-1) + Solution (i-1,j+1,k-1) + Solution (i-1,j-1,k-1) +
              Solution (i+1,j,k)     + Solution (i-1,j,k)     + Solution (i,j+1,k)     + Solution (i,j-1,k)     + 
              Solution (i+1,j+1,k)   + Solution (i+1,j-1,k)   + Solution (i-1,j+1,k)   + Solution (i-1,j-1,k)   +
              Solution (i+1,j,k+1)   + Solution (i-1,j,k+1)   + Solution (i,j+1,k+1)   + Solution (i,j-1,k+1)   + 
              Solution (i+1,j+1,k+1) + Solution (i+1,j-1,k+1) + Solution (i-1,j+1,k+1) + Solution (i-1,j-1,k+1) +
           // Center point of Inner and Outer faces
              Solution (i,j,k-1) + Solution (i,j,k+1) ) / 26.0;
   }

PDE & PDE::operator= ( const PDE & X )
   {
  // This function takes an existing object and copies.
  // It is ALWAYS a good idea to define both this and the copy constructor,
  //  even if they are not implemented (though we implement both in this code).

     I = X.I;
     J = X.J;
     K = X.K;

     Solution        = X.Solution;
     Exact_Solution  = X.Exact_Solution;
     Residual        = X.Residual;
     Right_Hand_Side = X.Right_Hand_Side;

     Mesh_Size = X.Mesh_Size;
     Grid_Size = X.Grid_Size;

     return *this;
   }


