//              ***********  C++ MG CODE  ************

#define MAIN_PROGRAM

#include "mg_main.h"

#define GET_INPUT_FROM_USER FALSE
#define SUBSTITUTE_COMMAND_LINE_INPUT_MG_LEVELS 8

#ifdef APP_GRAPHICS
// Global graphics window object
GraphicsWindow Graphics_Object;
#endif


// **************************************************************************
//                           GLOBAL FUNCTIONS
// **************************************************************************

double RHS_Function              ( double X , double Y , double Z );
double Exact_Solution_Function   ( double X , double Y , double Z );

// **************************************************************************
//       ************ OTHER FUNCTIONS FOR THIS PROGRAM **************
// **************************************************************************

double RHS_Function ( double X , double Y , double Z )
   {
  // This is a 2D function currently!

     const double PI = 3.14159265;
  // double XX = X * X * (X * X - 1.0);
  // double YY = Y * Y * (Y * Y - 1.0);
  // double ZZ = Z * Z * (Z * Z - 1.0);

     if (PDE::Problem_Dimension == 1)
        {
          return sin (X*PI) * (PI * PI);
        }
       else
        {
          if (PDE::Problem_Dimension == 2)
             {
            // return 3.0 * -2.0 * ((6*X*X-1)*YY + (6*Y*Y-1)*XX);
            // Rhs scaled by 1/3
            // return -2.0 * ((6*X*X-1)*YY + (6*Y*Y-1)*XX);
            // return 0.0;

               if (PDE::Fully_Variational)
                    return ( sin (X*PI) + sin(Y*PI) ) * (PI * PI) * 3.0;
                 else
                    return ( sin (X*PI) + sin(Y*PI) ) * (PI * PI);
             }
            else
             {
               if (PDE::Fully_Variational)
                    return ( sin (X*PI) + sin(Y*PI) + sin (Z*PI) ) * (PI * PI) * 6.0;
                 else
                    return ( sin (X*PI) + sin(Y*PI) + sin (Z*PI) ) * (PI * PI);
             }
        }
   }

double Exact_Solution_Function ( double X , double Y , double Z )
   {
  // This is a 2D function currently!
     const double PI = 3.14159265;
  // double XX = X * X * (X * X - 1.0);
  // double YY = Y * Y * (Y * Y - 1.0);
  // double ZZ = Z * Z * (Z * Z - 1.0);

  // Exact answer is: Exact_Answer = YY * XX;
  // return YY * XX;
  // return 0.0;
  // return sin (X*PI) * sin(Y*PI);

     if (PDE::Problem_Dimension == 1)
        {
          return sin (X*PI);
        }
       else
        {
          if (PDE::Problem_Dimension == 2)
             {
               return sin (X*PI) * sin(Y*PI);
             }
            else
             {
               return sin (X*PI) * sin(Y*PI) * sin(Z*PI);
             }
        }
   }

double Initial_Solution_Function ( double X , double Y , double Z )
   {
  // This is a 2D function currently!
     const double PI = 3.14159265;
     double XX = X * X * (X * X - 1.0);
     double YY = Y * Y * (Y * Y - 1.0);
     double ZZ = Z * Z * (Z * Z - 1.0);

     return YY * XX * ZZ;
   }

// **************************************************************************
//                        MAIN PROGRAM FUNCTION
// **************************************************************************

int main(int argc, char** argv)
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
     Index::setBoundsCheck (On); // Turns on P++ array bounds checking!

     int Number_Of_Processors = 8;
  // printf ("Number of processors = ");
  // scanf ("%d",&Number_Of_Processors);
     printf ("Number_Of_Processors = %d \n",Number_Of_Processors);

  // This can't be done before the initialization of the virtual machine
  // Communication_Manager::sync();

  // Need to uncomment this line to provide the correct path within the distribution
  // Optimization_Manager::Initialize_Virtual_Machine 
  // ("/n/elf0/dquinlan/A++P++.4.1.3.experimental/APPLICATIONS/p++_mg",
  //  Number_Of_Processors,argc,argv);
     Optimization_Manager::Initialize_Virtual_Machine 
  //   ("/n/c19s3/A++P++Source/A++P++Source/A++P++/APPLICATIONS/SUNULTRA/p_mg",
       ("/n/c19s0/dquinlan/A++P++Source/A++P++/APPLICATIONS/SUNULTRA/p_mg",
        Number_Of_Processors,argc,argv);

     printf ("Run P++ code (Number_Of_Processors = %d) \n",Number_Of_Processors);

  // Specify a ghost boundary width of 1 along the first axis
  // Partitioning_Type::DefaultInternalGhostCellWidth [0] = 1;
  // Partitioning_Type::SpecifyInternalGhostBoundaryWidths ( 1 );

  // Causes an error when used with 2D arrays
     Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths ( 1 );

  // Causes an error when used with 1D arrays
  // Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths ( 1 , 1 );

     Partitioning_Type::displayDefaultValues("In Main Program");

  // printf ("Exiting at TOP of main ... \n");
  // APP_ABORT();

#if 1
     int Multigrid_Level;   // Matrix Size log base 2

  // The parallel simulator can't easily handle terminal input (though P++ does)!
#if GET_INPUT_FROM_USER
     printf ("Max_Level = ");
     scanf ("%d",&Multigrid_Level);
     printf ("Max_Multigrid_Level = %d \n",Multigrid_Level);
#else
     Multigrid_Level = SUBSTITUTE_COMMAND_LINE_INPUT_MG_LEVELS;
#endif

     int Global_Grid_Size = int ( pow (2.0,double(Multigrid_Level)) + 1 );
     printf ("Global_Grid_Size = %d \n",Global_Grid_Size);

  // Specifiy options (override defaults!)
     PDE::Problem_Dimension           = 1;
  // PDE::Fully_Variational           = FALSE;
     PDE::Fully_Variational           = TRUE;
  // MG::Projection_Operator_Order    = MG::FIRST_ORDER_OPERATOR;
     MG::Projection_Operator_Order    = MG::SECOND_ORDER_OPERATOR;
     MG::Interpolation_Operator_Order = MG::SECOND_ORDER_OPERATOR;

     printf ("Problem is %d dimensional \n",PDE::Problem_Dimension);
     int N_Point_Operator = 0;
     if (PDE::Problem_Dimension == 1)
        {
          N_Point_Operator = 3;
        }
       else
        {
          if (PDE::Problem_Dimension == 2)
             {
               if (PDE::Fully_Variational)
                    N_Point_Operator = 9;
                 else
                    N_Point_Operator = 5;
             }
            else
             {
               if (PDE::Problem_Dimension == 3)
                    if (PDE::Fully_Variational)
                         N_Point_Operator = 27;
                      else
                         N_Point_Operator = 7;
             }
        }
     printf ("Using %d point operators \n",N_Point_Operator); 
     printf ("Using %d order Projection \n",MG::Projection_Operator_Order);
     printf ("Using %d order Interpolation \n",MG::Interpolation_Operator_Order);

#if 1
     printf ("\nNow build the PDE objects! \n\n");

  // APP_DEBUG = 5;
     PDE PDE_Problem ( Global_Grid_Size , RHS_Function , Exact_Solution_Function );

     printf ("PDE object Built! \n");

#if 1
  // APP_DEBUG = 5;
     PDE_Problem.Error ();
     for (int i=0; i <= 10; i++)
        {
          printf ("PDE::Solve()  iteration = %d \n",i);
#if 1
       // PDE_Problem.Red_Black_Relax ();
          PDE_Problem.Jacobi_Relax ();
       // PDE_Problem.Compute_Residuals ();
          PDE_Problem.Error ();
#endif
        }

  // exit(1);
#endif

  // printf ("Exiting in main() ... \n");
  // APP_ABORT();
#endif

#if 1
     printf ("\nNow build the MG objects! \n\n");
     MG MG_Problem ( Global_Grid_Size , RHS_Function , Exact_Solution_Function );

#if 1
  // Perturb Solution from exact solution!
  // MG_Problem.Multigrid_Level [MG_Problem.Max_Multigrid_Level].Initialize_Array_To_Function ( 
  //      MG_Problem.Multigrid_Level [MG_Problem.Max_Multigrid_Level].Solution , 
  //      Initial_Solution_Function );

     MG_Problem.Number_Of_MG_Iterations            = 5;
     MG_Problem.Number_Of_Relaxations_Down_V_Cycle = 1;
     MG_Problem.Number_Of_Relaxations_Up_V_Cycle   = 1;

#ifdef APP_GRAPHICS
     printf ("Build Graphics_Object \n");
  // GraphicsWindow Graphics_Object;
     GraphicsWindow Graphics_Object;
#if 1
     Graphics_Object.setProperty(TOPLABEL,"Fine Grid Solution");
     Graphics_Object.setProperty(CONTSTYLE,2);
     Graphics_Object.setPlotOptn(COLORPS);
     Graphics_Object.setProperty(NSTEPS,Number_Of_Processors-2);
     Graphics_Object.setPlotOptn(PSFILENM,"testcol.ps");

     MG_Problem.Solution = Communication_Manager::My_Process_Number;
     Graphics_Object.contour(MG_Problem.Solution);
     MG_Problem.Solution = 0.0;
  // MG_Problem.Right_Hand_Side.view("MG_Problem.Right_Hand_Side");
#endif

     Graphics_Object.setProperty(TOPLABEL,"Fine Grid Right Hand Side");
     Graphics_Object.setProperty(CONTSTYLE,2);
     Graphics_Object.setProperty(NSTEPS,20);
  // MG_Problem.Right_Hand_Side.display("MG_Problem.Right_Hand_Side");
     Graphics_Object.contour(MG_Problem.Right_Hand_Side);
#endif

     printf ("Now Solve the MG object! \n");
     MG_Problem.Solve();

#ifdef APP_GRAPHICS
     printf ("Call Graphics_Object.displayPlots () \n");
     Graphics_Object.displayPlots ();
#endif
#endif
#endif

#if 0
     printf ("\nBuild an array of MG objects and assign MG_Problem to it! \n\n");

  // Example of the use of C++ objects!
     MG Array_Of_MG_Problems [5];

     for (int i=0; i < 5; i++)
          Array_Of_MG_Problems [i] = MG_Problem;

     printf ("\nCall the Solver for one of the elements of the array of MG objects! \n\n");
     Array_Of_MG_Problems [0].Solve();
#endif

  // printf ("Solution \n");
  // MG_Problem.Solution.display("Cross-section of Solution");

  // printf ("Exact Solution \n");
  // MG_Problem.Exact_Solution.display("Cross-section of Exact_Solution");

#if 0
     printf ("Look at Cross-sections of data! \n");
     if (PDE::Problem_Dimension == 1)
        {
          printf ("1D Solution \n");
          MG_Problem.Solution.display("Cross-section of 1D Solution");
          printf ("3D Exact Solution \n");
          MG_Problem.Exact_Solution.display("Cross-section of 1D Exact_Solution");
        }
       else
        {
          if (PDE::Problem_Dimension == 2)
             {
               printf ("2D Solution \n");
               MG_Problem.Solution(Index(),Global_Grid_Size/2).display("Cross-section of 2D Solution");
               printf ("2D Exact Solution \n");
               MG_Problem.Exact_Solution(Index(),Global_Grid_Size/2).display("Cross-section of 2D Exact_Solution");
             }
            else
             {
               printf ("3D Solution \n");
               MG_Problem.Solution(Index(),Global_Grid_Size/2,Global_Grid_Size/2).
                    display("Cross-section of 3D Solution");
               printf ("3D Exact Solution \n");
               MG_Problem.Exact_Solution(Index(),Global_Grid_Size/2,Global_Grid_Size/2).
                    display("Cross-section of 3D Exact_Solution");
             }
        }
#endif

     printf ("\n\n");
     printf ("Program Terminated Normally! \n");
     Optimization_Manager::Exit_Virtual_Machine ();
     printf ("Virtual Machine exited! \n");
  // APP_DEBUG = 6;
#endif

     return 0;
   }


