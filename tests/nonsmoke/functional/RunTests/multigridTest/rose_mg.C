template < class T, int Template_Dimension > class Array_Descriptor_Type;
//              ***********  C++ MG CODE  ************



#include "rose_mg_main.h"



#define DEBUG 0



#define PPP_REFERENCE_BUG_FIXED  TRUE



// ****************************************************************************


//                        MULTIGRID C++ CLASS


// ****************************************************************************
const int MG::FIRST_ORDER_OPERATOR=(1); 
const int MG::SECOND_ORDER_OPERATOR=(2); 

Boolean MG::Projection_Operator_Order=(2); 
Boolean MG::Interpolation_Operator_Order=(2); 

MG::~MG()
   { 
     delete []/*ghost_this*/Multigrid_Level; 
} 
     
// printf ("MG DESTRUCTOR called \n");



MG::MG() : Multigrid_Level(new Single_MG_Level [11])
   { 
   
// Default constructor for compiler!

     if (FALSE)
          { printf("MG CONSTRUCTOR called \n"); } 

     /*ghost_this*/COARSEST_LEVEL = 1; 
} 

 MG & MG::operator=(const MG & Rhs)
   { 
   
// It is always a good idea to define the equals operator and the 

   
// copy constructor (even if they are not implemented).

   
// Otherwise the compliler will build them for you and it will

   
// not always do it the way you wanted (shallow copies are made).

   
// This function takes an existing object and copies.


     if (FALSE)
          { printf("Inside of MG::operator=! \n"); } 

     /*ghost_this*/COARSEST_LEVEL = Rhs.COARSEST_LEVEL; 

     /*ghost_this*/Max_Multigrid_Level = Rhs.Max_Multigrid_Level; 
     /*ghost_this*/Number_Of_MG_Iterations = Rhs.Number_Of_MG_Iterations; 
     /*ghost_this*/Number_Of_Relaxations_Down_V_Cycle = Rhs.Number_Of_Relaxations_Down_V_Cycle; 
     /*ghost_this*/Number_Of_Relaxations_Up_V_Cycle = Rhs.Number_Of_Relaxations_Up_V_Cycle; 

     /*ghost_this*/Solution=Rhs.Solution; 
     /*ghost_this*/Right_Hand_Side=Rhs.Right_Hand_Side; 
     /*ghost_this*/Exact_Solution=Rhs.Exact_Solution; 
     /*ghost_this*/Residual=Rhs.Residual; 
     /*ghost_this*/Mesh_Size = Rhs.Mesh_Size; 
     /*ghost_this*/Grid_Size = Rhs.Grid_Size; 

     for (int i=/*ghost_this*/Max_Multigrid_Level; i >= /*ghost_this*/COARSEST_LEVEL; i--)
          { (/*ghost_this*/Multigrid_Level[i])=(Rhs.Multigrid_Level)[i]; } 

     return *this;
 } 

MG::MG(int Local_Grid_Size) : COARSEST_LEVEL(1), Multigrid_Level(new Single_MG_Level [11])


   { 
   
// Simple constructor for MG object.


     /*ghost_this*/Construct_MG(Local_Grid_Size); 
} 

MG::MG(int Local_Grid_Size,double (* Function_RHS)(double , double , double )) : COARSEST_LEVEL(1), Multigrid_Level(new Single_MG_Level [11])



   { 
   
// Constructor for MG object (requires function defining the RHS).


     /*ghost_this*/Construct_MG(Local_Grid_Size); 
     /*ghost_this*/Initialize_MG(Function_RHS); 
} 

MG::MG(int Local_Grid_Size,double (* Function_RHS)(double , double , double ),double (* Exact_Solution_Function)(double , double , double )) : COARSEST_LEVEL(1), Multigrid_Level(new Single_MG_Level [11])




   { 
   
// Constructor for MG object (requires function defining the RHS and the exact solution).


     /*ghost_this*/Construct_MG(Local_Grid_Size); 
     
// printf ("Call Initialize_MG \n");

     /*ghost_this*/Initialize_MG(Function_RHS); 
     
// printf ("Call Multigrid_Level [Max_Multigrid_Level].Setup_PDE_Exact_Solution \n");

     (/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Setup_PDE_Exact_Solution(Exact_Solution_Function); 
     
// printf ("Call DONE: Multigrid_Level [Max_Multigrid_Level].Setup_PDE_Exact_Solution \n");



#if 1


#if 1

     
// Setup RHS so we can compute the exact solution with zero discretization error!

     ((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Solution)=(/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Exact_Solution; 
     ((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Right_Hand_Side)
     =0.0; 
     (/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Compute_Residuals(); 
     ((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Right_Hand_Side)
     =-((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Residual);      
// Multigrid_Level [Max_Multigrid_Level].Right_Hand_Side =  Multigrid_Level [Max_Multigrid_Level].Residual;

     ((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Residual)=0.0; 
     ((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Solution)=0.0; 
} 

#else

     
// Multigrid_Level [Max_Multigrid_Level].Solution = Multigrid_Level [Max_Multigrid_Level].Exact_Solution;



     
// Multigrid_Level [Max_Multigrid_Level].Solution (Grid_Size/2) = 1.0;





#endif


#endif



void MG::Construct_MG(int Fine_Grid_Size)
   { 
   
// Construction is seperate from Initialization since in P++ the construction may be deferred

   
// until after all the objects are know at runtime, then the objects are built (all the arrays

   
// data allocated) after a intermediate load balancing step.  This the Construction functions for an 

   
// object must not assign values to the array data (this is done in the initialization function).

   
// This function specifies the size of all of the array objects and does not assign values to the 

   
// array data in this object.


   
// Construct the MG object:

   
//    1) Setup scalar values.

   
//    2) Construct PDE objects.

   
//    3) Setup Index objects.


     /*ghost_this*/Grid_Size = Fine_Grid_Size; 

     int Number_Of_Multigrid_Levels=(int )ceil(log((double )(Fine_Grid_Size - 1)) / log(2.0)
      - 0.1); 
     
// Error Checking!

     int Expected_Grid_Size=(int )pow(2.0,(double )Number_Of_Multigrid_Levels)
      + 1; 
     if (/*ghost_this*/Grid_Size != Expected_Grid_Size)
        { 
          printf("ERROR: Inside of MG::Construct_MG: Grid_Size = %d                Expected_Grid_Size = %d \n",
          /*ghost_this*/Grid_Size,Expected_Grid_Size); 
          exit(1); 
     } 

     /*ghost_this*/Max_Multigrid_Level = Number_Of_Multigrid_Levels; 

     
// Note that the ZERO element of the array of Single_MG_Levels is not used!

     for (int Multigrid_Level_Index=Number_Of_Multigrid_Levels; Multigrid_Level_Index
      >= /*ghost_this*/COARSEST_LEVEL; Multigrid_Level_Index--)

        { 
          int Problem_Size=(int )pow(2.0,(double )Multigrid_Level_Index) + 1; 
          (/*ghost_this*/Multigrid_Level[Multigrid_Level_Index]).Construct_PDE(Problem_Size); 
     } 

          
// Default values!

     /*ghost_this*/Number_Of_MG_Iterations = 1; 
     /*ghost_this*/Number_Of_Relaxations_Down_V_Cycle = 2; 
     /*ghost_this*/Number_Of_Relaxations_Up_V_Cycle = 1; 

     /*ghost_this*/Mesh_Size = (/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Mesh_Size; 
     /*ghost_this*/Grid_Size = (/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Grid_Size; 

     /*ghost_this*/I=(/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).I; 
     /*ghost_this*/J=(/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).J; 
     /*ghost_this*/K=(/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).K; 
} 

void MG::Initialize_MG(double (* Function_RHS)(double , double , double ))
   { 
   
// Construction is seperate from Initialization since in P++ the construction may be deferred

   
// until after all the objects are know at runtime, then the objects are built (all the arrays

   
// data allocated) after a intermediate load balancing step.  This the Construction functions for an 

   
// object must not assign values to the array data (this is done in the initialization function).

   
// This function assigns values to the array data in this object and does not specify the size

   
// of any of the array objects.


   
// Initialize the MG object:

   
//     1) Initialize all the Levels.

   
//     2) Initialize the references in the MG object to the finest level.


     for (int Multigrid_Level_Index=/*ghost_this*/Max_Multigrid_Level; Multigrid_Level_Index
      >= /*ghost_this*/COARSEST_LEVEL; Multigrid_Level_Index--)

        { 
        
// Bug fix to avoid uninitialized memory reads!

        
// Multigrid_Level [Multigrid_Level_Index].Initialize_PDE ( Function_RHS );

        
// if (Multigrid_Level_Index < Max_Multigrid_Level)


          if (Multigrid_Level_Index <= /*ghost_this*/Max_Multigrid_Level)
             { 
               ((/*ghost_this*/Multigrid_Level[Multigrid_Level_Index]).Solution).fill(0.0); 
               ((/*ghost_this*/Multigrid_Level[Multigrid_Level_Index]).Right_Hand_Side).fill(0.0); 
               ((/*ghost_this*/Multigrid_Level[Multigrid_Level_Index]).Exact_Solution).fill(0.0); 
               ((/*ghost_this*/Multigrid_Level[Multigrid_Level_Index]).Residual).fill(0.0); 
          } 

          printf("Call Multigrid_Level [%d].Initialize_PDE \n",Multigrid_Level_Index); 
          
// Multigrid_Level [Multigrid_Level_Index].Initialize_PDE ( Function_RHS );

          
//                                         Initialize_RHS

          (/*ghost_this*/Multigrid_Level[Multigrid_Level_Index]).Setup_RHS(); 
          printf("Call DONE: Multigrid_Level [%d].Initialize_PDE \n",Multigrid_Level_Index); 
     } 

          
// Now build Solution as references to Fine grid Solution!


     /*ghost_this*/Solution.reference((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Solution); 
     /*ghost_this*/Right_Hand_Side.reference((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Right_Hand_Side); 
     /*ghost_this*/Exact_Solution.reference((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Exact_Solution); 
     /*ghost_this*/Residual.reference((/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Residual); 
} 
     
// Lock these so that they can't be reassigned by accident

     
// to a different size or a different view!


     
// Concept of reference locking is not implemented yet (all references are locked)

     
// later it will be possible to unlock a reference so assignment can change the

     
// array object used as a reference but not the reference. It is unclear to me 

     
// if this is important enough to justify it's introduction (so I will hold off).


     
// Solution.lock();

     
// Right_Hand_Side.lock();

     
// Exact_Solution.lock();

     
// Residual.lock();



void MG::Setup_Exact_Solution(double (* Exact_Solution_Function)(double , double , double ))
   { 
     (/*ghost_this*/Multigrid_Level[/*ghost_this*/Max_Multigrid_Level]).Setup_PDE_Exact_Solution(Exact_Solution_Function); 
} 

void MG::Residual_Projection(int Level)
   { 
     (/*ghost_this*/Multigrid_Level[Level]).Projection((/*ghost_this*/Multigrid_Level[Level]).Residual,
     (/*ghost_this*/Multigrid_Level[(Level - 1)]).Right_Hand_Side,MG::Projection_Operator_Order); 


     
// Residual weighting for Red-Black relaxation!

     
// It is not a great point of this design that this is done here!

     
// Scaling for the injection operator only!

     if (MG::Projection_Operator_Order == 1)
          { ((/*ghost_this*/Multigrid_Level[(Level - 1)]).Right_Hand_Side)*=0.5; } 

          
// ZERO this space after use!

     ((/*ghost_this*/Multigrid_Level[Level]).Residual).fill(0.0); 
} 

void MG::Interpolate_And_Add(int Level)
   { 
   
// This function implements the interpolation and addition of the 

   
// solution from a coarser grid to the solution on a finer grid.


     

     
// Use the residual as a temporay storage! Sort of dangerous (the way aliasing is in FORTRAN)!





     
// Compute and add interpolent.








     
// APP_DEBUG = 5;




     
// printf ("Exiting in MG::Interpolate_And_Add \n");

     
// APP_ABORT();





     
// Reset solution of coarse grid to ZERO! Since zero is the best initial guess 

     
// for the solution to the residual equation we will pose on the next iteration!



     
// This is space borrowed from the Residual on the finer level! ZERO IT ON EXIT!



     
// APP_DEBUG = 5;





     
// almost always equal to 1 (unless debugging code)

     
// int Base_Level = 2;

     
// int Base_Level = Max_Level;

     
// int Base_Level = COARSEST_LEVEL+1;







     
// Solve this level (few sweeps of relaxation).




     
// Compute and project residual




     
// Recursive call to V_Cycle




     
// Smoothing to relax components excited by interpolation!




     
// Interpolate solution to residual equation posed on

     
// this level and projected previously to coarser grid.



     
// APP_DEBUG = 5;



     
// printf ("Exiting after Interpolate_And_Add \n");

     
// APP_ABORT();






     
// Full multigrid cycle (actually the coarse grid problem is

     
// not posed so this is incomplete).

















     
// Build a reference variable to simplify the code and avoid copying!




     
// Compute the L2_Norm seperately so we can store the result

     
// for the computation of the convergence rate later!


     
// APP_DEBUG = 5;

     
// Interior_Of_Residual_Grid.view("Interior_Of_Residual_Grid");




     
// printf ("Exiting After L2_Norm in MG::Solve() \n");

     
// exit (1);
















     
// Reuse our reference to the interior of the residual stored in "Interior_Of_Residual_Grid"







     
// Store the history so we can compute the convergence rate!
doubleArray Fine_Grid_Interpolent; 
     Fine_Grid_Interpolent.reference((/*ghost_this*/Multigrid_Level[(Level + 1)]).Residual); 
     Fine_Grid_Interpolent.fill(0.0); 
     (/*ghost_this*/Multigrid_Level[(Level + 1)]).Interpolate(Fine_Grid_Interpolent,(/*ghost_this*/Multigrid_Level[Level]).Solution,
     MG::Interpolation_Operator_Order); 
     if (
     FALSE)
          { 
          ((/*ghost_this*/Multigrid_Level[(Level + 1)]).Solution).display("BEFORE ADD"); } 
     ((/*ghost_this*/Multigrid_Level[(Level + 1)]).Solution)+=Fine_Grid_Interpolent; 
     if (
     FALSE)
          { 
          ((/*ghost_this*/Multigrid_Level[(Level + 1)]).Solution).display("AFTER ADD"); } 
     ((/*ghost_this*/Multigrid_Level[Level]).Solution).fill(0.0); 
     Fine_Grid_Interpolent.fill(0.0); 
} 
void MG::V_Cycle(int Level,int Max_Level)
   { 
     int Base_Level=/*ghost_this*/COARSEST_LEVEL; 
     if (
     FALSE)
          { 
          printf("Inside of MG::V_Cycle ( Level=%d , Max_Level=%d ) \n",Level,Max_Level); } 
     if (
     Level > Base_Level)
        { 
          for (
int i=1; i <= /*ghost_this*/Number_Of_Relaxations_Down_V_Cycle; i++)
               { 
               (/*ghost_this*/Multigrid_Level[Level]).Solve(); } 
          (/*ghost_this*/Multigrid_Level[Level]).Compute_Residuals(); 
          /*ghost_this*/Residual_Projection(Level); 
          /*ghost_this*/V_Cycle(Level - 1,Max_Level); 
     } 
     for (
int j=1; j <= /*ghost_this*/Number_Of_Relaxations_Up_V_Cycle; j++)
          { 
          (/*ghost_this*/Multigrid_Level[Level]).Solve(); } 
     if (
     Level < Max_Level)
        { 
          /*ghost_this*/Interpolate_And_Add(Level); 
     } 
} 
void MG::Full_MultiGrid_Cycle()
   { 
     for (
int FMG_Level=2; FMG_Level <= /*ghost_this*/Max_Multigrid_Level; ++FMG_Level)
        { 
          /*ghost_this*/V_Cycle(FMG_Level,FMG_Level); 
          /*ghost_this*/Interpolate_And_Add(FMG_Level); 
     } 
} 
void MG::Solve()
   { 
     double L2_Norm_Before_MG=1.0; 
     double L2_Norm_After_MG=1.0; 
     int Max_Level_To_Solve=/*ghost_this*/Max_Multigrid_Level; 
     (/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).Compute_Residuals(); 
     doubleArray Interior_Of_Residual_Grid; 
     Interior_Of_Residual_Grid.reference(((/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).Residual)(/*ghost_this*/I,
     /*ghost_this*/J,/*ghost_this*/K)); 
     L2_Norm_Before_MG = ((/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).L2_Norm)(Interior_Of_Residual_Grid); 
     printf("Initial L2_Norm Residual = %e Max Residual = %e Min Residual = %e \n",L2_Norm_Before_MG,
     max(Interior_Of_Residual_Grid),min(Interior_Of_Residual_Grid)); 
     (/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).Error(); 
     printf("Now iterate over the number of V_Cycles \n"); 
     for (
int i=1; i <= /*ghost_this*/Number_Of_MG_Iterations; i++)
        { 
          /*ghost_this*/V_Cycle(Max_Level_To_Solve,Max_Level_To_Solve); 
          (/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).Error(); 
          (/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).Compute_Residuals(); 
          L2_Norm_After_MG = ((/*ghost_this*/Multigrid_Level[Max_Level_To_Solve]).L2_Norm)(Interior_Of_Residual_Grid); 
          printf("Res L2_Norm = %e Sum Squ Res = %e MG Convergence Rate = %e \n",L2_Norm_After_MG,
          sum(Interior_Of_Residual_Grid*Interior_Of_Residual_Grid),L2_Norm_After_MG / L2_Norm_Before_MG); 
          L2_Norm_Before_MG = L2_Norm_After_MG; 
     } 
} 
