template < class T, int Template_Dimension > class Array_Descriptor_Type;
//              ***********  C++ PDE CODE  ************



#include "rose_mg_main.h"



#define DEBUG 0



#ifdef GRAPHICS



#endif



// ****************************************************************************


//                  PDE (Uniform Square Grid) C++ CLASS


// ****************************************************************************



// Static class data members must be declared!
int PDE::Problem_Dimension=(2); 
Boolean PDE::Fully_Variational=(1); 


PDE::PDE()
   { 
   
// Nothing to do here since the default constructors

   
// for all our data objects are called automatically!


   
// Provide some default values for scalar data fields!

     /*ghost_this*/Mesh_Size = 1.0; 
     /*ghost_this*/Grid_Size = 1; 
} 

PDE::~PDE()
   { } 
   
// A very simple destructor!

   
// Nothing to delete since we have no pointers to other objects!



PDE::PDE(int Local_Grid_Size)
   { 
   
// This is a very simple constructor, it allows the creation of an object

   
// with arrays of a specific size (then the Rhs can be setup seperately).


     /*ghost_this*/Construct_PDE(Local_Grid_Size); 
} 

PDE::PDE(int Local_Grid_Size,double (* Function_RHS)(double , double , double ),double (* Exact_Solution_Function)(double , double , double ))


   { 
   
// This constructor for the PDE object assumes the existence of an exact solution and

   
// allows it to be passed into the object so that error norms will be meaning full.


     printf("Call: Construct_PDE  ( Local_Grid_Size = %d ) \n",Local_Grid_Size); 
     /*ghost_this*/Construct_PDE(Local_Grid_Size); 
     printf("CALL: Initialize_PDE ( Function_RHS ) \n"); 
     /*ghost_this*/Initialize_PDE(Function_RHS); 
     /*ghost_this*/Setup_PDE_Exact_Solution(Exact_Solution_Function); 


#if 1

     
// Setup RHS so we can compute the exact solution with zero discretization error!

     /*ghost_this*/Solution=/*ghost_this*/Exact_Solution; 
     /*ghost_this*/Right_Hand_Side=0.0; 
     printf("CALL: Compute_Residuals () \n"); 
     /*ghost_this*/Compute_Residuals(); 
     
// Temp code!

     
// Residual = 0.0;


     
// APP_DEBUG = 5;

     
// Right_Hand_Side.view("Right_Hand_Side");

     
// Residual.view("Residual");

     /*ghost_this*/Right_Hand_Side=-/*ghost_this*/Residual; 

#endif


     /*ghost_this*/Residual=0.0; 
     /*ghost_this*/Solution=0.0; 
} 

#if 0






#endif


     
// printf ("Exiting in PDE::PDE! \n");

     
// APP_ABORT();



void PDE::Setup_PDE_Exact_Solution(double (* Exact_Solution_Function)(double , double , double ))
   { 
   
// Setup the Exact Solution with the values from the function defineing the exact solution!


     /*ghost_this*/Initialize_Array_To_Function(/*ghost_this*/Exact_Solution,Exact_Solution_Function); 
} 

void PDE::Setup_RHS()
   { 
     const double PI=3.14159265; 

     
// If we compute these once here, then we can avoid 

     
// redundent function call overhead inside the loop structure

     int Base_K=/*ghost_this*/Right_Hand_Side.getBase(2); 
     int Base_J=/*ghost_this*/Right_Hand_Side.getBase(1); 
     int Base_I=/*ghost_this*/Right_Hand_Side.getBase(0); 

     int Bound_K=/*ghost_this*/Right_Hand_Side.getBound(2); 
     int Bound_J=/*ghost_this*/Right_Hand_Side.getBound(1); 
     int Bound_I=/*ghost_this*/Right_Hand_Side.getBound(0); 

     if (PDE::Problem_Dimension == 1)
        { 
        
// return sin (X*PI) * (PI * PI);

          











          
// Temp_X.display("Temp_X");

          
// Temp_Y.display("Temp_Y");



          
// return ( sin (X*PI) + sin(Y*PI) ) * (PI * PI) * 3.0;

          
// Right_Hand_Side = sin(Temp_X*(Mesh_Size*PI)) * sin(Temp_Y*(Mesh_Size*PI)) * (PI * PI) * 3.0; 






          
// return ( sin (X*PI) + sin(Y*PI) ) * (PI * PI);

          
// Right_Hand_Side = sin(Temp_X*PI) * sin(Temp_Y*PI) * (PI * PI); 





          
// Temp_X.display("Temp_X");

          
// Temp_Y.display("Temp_Y");



          
// printf ("Now do the assignment to the Right_Hand_Side \n");



          
// double yy = Temp_Y(j);


          
// printf ("yy = %f \n",yy);

          
// Right_Hand_Side (range_I,j+Base_J).display("Right_Hand_Side (range_I,j+Base_J)");

          
// Temp_X.display("Temp_X");




          
// Right_Hand_Side.display("Right_Hand_Side");


          
// printf ("Exiting in PDE::Setup_RHS \n");

          
// APP_ABORT();












          
// return ( sin (X*PI) + sin(Y*PI) + sin (Z*PI) ) * (PI * PI) * 6.0;

          
// Right_Hand_Side = sin(Temp_X*PI) * sin(Temp_Y*PI) * sin(Temp_Z*PI) * (PI * PI) * 6.0;







          
// return ( sin (X*PI) + sin(Y*PI) + sin (Z*PI) ) * (PI * PI);

          
// Right_Hand_Side = sin(Temp_X*PI) * sin(Temp_Y*PI) * sin(Temp_Z*PI) * (PI * PI); 























          
// This function initializes the input array using the input function.  Notice that

          
// it is independent of any particular base and bound of the array, it queries the

          
// array object for its base and bound (stride is assumed to be 1 in this function).

          
// This sort of scalar indexing is slower than normal FORTRAN of C, but this setup is

          
// not a significant part of the solver so it makes little difference.  Scalar indexing

          
// is inlined and because of this can be as efficient as FORTRAN indexing with some C++

          
// compilers. This could be implemented differently to avoidthe function call and in doing so

          
// the inlined code should be as efficient as FORTRAN.







#if 1

          
// This used P++ functionality to just operate upon the local pieces of the distributed array objects

          
// This will also execute in A++.


          
// If we compute these once here, then we can avoid 

          
// redundent function call overhead inside the loop structure























          
// printf ("Before: X=%f   Y=%f   Z=%f \n",X,Y,Z);


          
// printf ("After: X=%f   Y=%f   Z=%f  Array(%d,%d,%d) = %f \n",X,Y,Z,i,j,k,Array(i,j,k));






#else

          
// If we compute these once here, then we can avoid 

          
// redundent function call overhead inside the loop structure




          
// int Base_J = Base_K;

          
// int Base_I = Base_K;






          
// printf ("Base_I  = %d  Base_J  = %d  Base_K  = %d \n",Base_I,Base_J,Base_K);

          
// printf ("Bound_I = %d  Bound_J = %d  Bound_K = %d \n",Bound_I,Bound_J,Bound_K);












          
// printf ("Before: X=%f   Y=%f   Z=%f \n",X,Y,Z);


          
// printf ("After: X=%f   Y=%f   Z=%f  Array(%d,%d,%d) = %f \n",X,Y,Z,i,j,k,Array(i,j,k));







#endif


          
// Array.getSerialArrayPointer()->display("Local Array Data");










          
// This variable may be set to any value, it defines the base of the 

          
// arrays that are construccted in this object.  It was uses as a test to 

          
// debug the ability in A++ to has arrays with any base (positive or negative).

          
// Range is -2,147,483,647 to 2,147,483,647-arraysize!

          
// int Alt_Base = 1000000000;

          
// int Alt_Base = 0;

          
// int Alt_Base = 1;



          
// Initialize objects scalar fields




          
// Since the default constructor already built the PDE objects it already called

          
// the default constructors for the array objects!  So we have to redimension the existing

          
// array objects!


          
// APP_DEBUG = 5;




          
// printf ("Turn ON APP_DEBUG = 5 in PDE::Construct_PDE \n");

          
// APP_DEBUG = 6;


          
// APP_DEBUG = 0;


          
// printf ("Exiting in PDE::Construct_PDE ... \n");

          
// APP_ABORT();












































          
// printf ("Set Solution(I) = 0 \n");

          
// I.display("I");

          
// Solution.view("Solution");

          
// Solution(I).view("Solution(I)");

          
// Solution(I) = 0;


          
// printf ("Exiting in PDE::Construct_PDE ... \n");

          
// APP_ABORT();











          
// Update the ghost boundaries to make them consistant and also

          
// avoid the purify UMR warning which was forcing a read of the 

          
// uninitialized ghost boundaries

          
// This is now done in the P++ redim function!


          
// Right_Hand_Side.updateGhostBoundaries();

          
// Residual.updateGhostBoundaries();

          
// Exact_Solution.updateGhostBoundaries();


          
// Now scale the Right_Hand_Side!

          
// Right_Hand_Side *= (Mesh_Size * Mesh_Size);






          
// This function implements Jacobi relaxation.

          
// This is the simple case of using the existing I,J index objects for the

          
// interior of the grid and handing them to the Five_Point_Stencil member function.






































          
// This function implements red/black relaxation

          
// on the interior of the array and is further restricted

          
// to use with odd numbered dimension lengths.


          
// Warning: For odd-numbered dimensinons only!

          
//          Relaxation on INTERIOR only!




          
// APP_DEBUG = 5;

          
// printf ("Inside of PDE::Red_Black_Relax -- Alt_Base = %d \n",Alt_Base);
doubleArray Temp_X(/*ghost_this*/Right_Hand_Side.getLength(0)); 
          Temp_X.seqAdd(0.0); 
          /*ghost_this*/Right_Hand_Side=sin((Temp_X*(/*ghost_this*/Mesh_Size * PI)))*(PI
           * PI); 
     } 
     else if (
          PDE::Problem_Dimension == 2)
             { 
               doubleArray Temp_X(/*ghost_this*/Right_Hand_Side.getLength(0)); 
               doubleArray Temp_Y(/*ghost_this*/Right_Hand_Side.getLength(0)); 
               Temp_X.seqAdd(0.0); 
               Temp_Y.seqAdd(0.0); 
               if (
               PDE::Fully_Variational)
                  { 
                    Temp_X=sin((Temp_X*(PI * /*ghost_this*/Mesh_Size)))*3.0; 
                    Temp_Y=sin((Temp_Y*(PI * /*ghost_this*/Mesh_Size)))*(PI * PI); 
               } 
               else 
                  { 
                    Temp_X=sin(Temp_X*(PI * /*ghost_this*/Mesh_Size)); 
                    Temp_Y=sin((Temp_Y*(PI * /*ghost_this*/Mesh_Size)))*(PI * PI); 
               } 
               Range range_I(/*ghost_this*/Right_Hand_Side.getBase(0),/*ghost_this*/Right_Hand_Side.getBound(0)); 
               for (
int j=Temp_Y.getBase(0); j <= (Temp_Y.getBound)(0); j++)
                  { 
                    double yy=sin((j * (PI * /*ghost_this*/Mesh_Size))) * (PI * PI); 
                    /*ghost_this*/Right_Hand_Side(range_I,Internal_Index((j + Base_J)))
                    =Temp_X*yy; 
               } 
     } 
     else 
             { 
               doubleArray Temp_X(/*ghost_this*/Right_Hand_Side.getLength(0)); 
               doubleArray Temp_Y(/*ghost_this*/Right_Hand_Side.getLength(0)); 
               doubleArray Temp_Z(/*ghost_this*/Right_Hand_Side.getLength(0)); 
               Temp_X.seqAdd(0.0); 
               Temp_Y.seqAdd(0.0); 
               Temp_Z.seqAdd(0.0); 
               if (
               PDE::Fully_Variational)
                  { 
                    Temp_X=sin(Temp_X*PI); 
                    Temp_Y=sin(Temp_Y*PI); 
                    Temp_Z=sin((Temp_Z*PI))*((PI * PI) * 6.0); 
               } 
               else 
                  { 
                    Temp_X=sin(Temp_X*PI); 
                    Temp_Y=sin(Temp_Y*PI); 
                    Temp_Z=sin((Temp_Z*PI))*(PI * PI); 
               } 
               for (
int k=Base_K; k <= Bound_K; k++)
                  { 
                    for (
int j=Base_J; j <= Bound_J; j++)
                       { 
                         for (
int i=Base_I; i <= Bound_I; i++)
                            { 
                              /*ghost_this*/Right_Hand_Side(i,j,k) = (Temp_X((i - Base_I))
                               * Temp_Y((j - Base_J))) * Temp_Z((k - Base_K)); 
                         } 
                    } 
               } 
     } 
} 
void PDE::Initialize_Array_To_Function(doubleArray & Array,double (* Primative_Function)(double , double , double ))
   { 
     double X; 
double Y; 
double Z=0.0; 
     if (
     FALSE)
          { 
          printf("Inside of PDE::Initialize_Array_To_Function \n"); } 
     int Base_K=Array.getBase(2); 
     int Base_J=Array.getBase(1); 
     int Base_I=Array.getBase(0); 
     int LocalBase_K=Array.getLocalBase(2); 
     int LocalBase_J=Array.getLocalBase(1); 
     int LocalBase_I=Array.getLocalBase(0); 
     int Bound_K=Array.getLocalBound(2); 
     int Bound_J=Array.getLocalBound(1); 
     int Bound_I=Array.getLocalBound(0); 
     Optimization_Manager::setOptimizedScalarIndexing(On); 
     for (
int k=LocalBase_K; k <= Bound_K; k++)
        { 
          for (
int j=LocalBase_J; j <= Bound_J; j++)
             { 
               for (
int i=LocalBase_I; i <= Bound_I; i++)
                  { 
                    X = /*ghost_this*/Mesh_Size * (double )(i - Base_I); 
                    Y = /*ghost_this*/Mesh_Size * (double )(j - Base_J); 
                    Z = /*ghost_this*/Mesh_Size * (double )(k - Base_K); 
                    Array(i,j,k) = (*Primative_Function)(X,Y,Z); 
               } 
          } 
     } 
     Optimization_Manager::setOptimizedScalarIndexing(Off); 
} 
void PDE::Construct_PDE(int Local_Grid_Size)
   { 
     if (
     FALSE)
          { 
          printf("Inside of PDE::Construct_PDE \n"); } 
     printf("Local_Grid_Size = %d \n",Local_Grid_Size); 
     int Alt_Base=-1000; 
     /*ghost_this*/Grid_Size = Local_Grid_Size; 
     /*ghost_this*/Mesh_Size = 1.0 / (double )(Local_Grid_Size - 1); 
     if (
     PDE::Problem_Dimension == 1)
        { 
          /*ghost_this*/I=Index(1 + Alt_Base,/*ghost_this*/Grid_Size - 2,1); 
          /*ghost_this*/Solution.redim(/*ghost_this*/Grid_Size); 
          /*ghost_this*/Right_Hand_Side.redim(/*ghost_this*/Grid_Size); 
          /*ghost_this*/Residual.redim(/*ghost_this*/Grid_Size); 
          /*ghost_this*/Exact_Solution.redim(/*ghost_this*/Grid_Size); 
     } 
     else if (
          PDE::Problem_Dimension == 2)
             { 
               /*ghost_this*/I=Index(1 + Alt_Base,/*ghost_this*/Grid_Size - 2,1); 
               /*ghost_this*/J=/*ghost_this*/I; 
               /*ghost_this*/Solution.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size); 
               /*ghost_this*/Right_Hand_Side.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size); 
               /*ghost_this*/Residual.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size); 
               /*ghost_this*/Exact_Solution.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size); 
     } 
          else if (
               PDE::Problem_Dimension == 3)
                  { 
                    /*ghost_this*/I=Index(1 + Alt_Base,/*ghost_this*/Grid_Size - 2,1); 
                    /*ghost_this*/J=/*ghost_this*/I; 
                    /*ghost_this*/K=/*ghost_this*/I; 
                    /*ghost_this*/Solution.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size,
                    /*ghost_this*/Grid_Size); 
                    /*ghost_this*/Right_Hand_Side.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size,
                    /*ghost_this*/Grid_Size); 
                    /*ghost_this*/Residual.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size,
                    /*ghost_this*/Grid_Size); 
                    /*ghost_this*/Exact_Solution.redim(/*ghost_this*/Grid_Size,/*ghost_this*/Grid_Size,
                    /*ghost_this*/Grid_Size); 
          } 
          else 
                  { 
                    printf("ERROR: Problem_Dimension > 3! \n"); 
                    exit(1); 
          } 
     printf("Call setBase! \n"); 
     /*ghost_this*/Solution.setBase(Alt_Base); 
     /*ghost_this*/Right_Hand_Side.setBase(Alt_Base); 
     /*ghost_this*/Residual.setBase(Alt_Base); 
     /*ghost_this*/Exact_Solution.setBase(Alt_Base); 
     if (
     FALSE)
          { 
          printf("Leaving PDE::Construct_PDE \n"); } 
} 
void PDE::Initialize_PDE(double (* Function_RHS)(double , double , double ))
   { 
     printf("TOP of Initialize_PDE! \n"); 
     /*ghost_this*/Initialize_Array_To_Function(/*ghost_this*/Right_Hand_Side,Function_RHS); 
     /*ghost_this*/Solution.updateGhostBoundaries(); 
     printf("BOTTOM of Initialize_PDE! \n"); 
} 
void PDE::Jacobi_Relax()
   { 
     if (
     PDE::Problem_Dimension == 1)
        { 
          /*ghost_this*/Three_Point_Stencil(/*ghost_this*/I); 
     } 
     else if (
          PDE::Problem_Dimension == 2)
             { 
               if (
               PDE::Fully_Variational)
                  { 
                    /*ghost_this*/Nine_Point_Stencil(/*ghost_this*/I,/*ghost_this*/J); 
               } 
               else 
                  { 
                    /*ghost_this*/Five_Point_Stencil(/*ghost_this*/I,/*ghost_this*/J); 
               } 
     } 
          else if (
               PDE::Problem_Dimension == 3)
                  { 
                    if (
                    PDE::Fully_Variational)
                       { 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(/*ghost_this*/I,/*ghost_this*/J,
                         /*ghost_this*/K); 
                    } 
                    else 
                       { 
                         /*ghost_this*/Seven_Point_Stencil(/*ghost_this*/I,/*ghost_this*/J,
                         /*ghost_this*/K); 
                    } 
          } 
} 
void PDE::Red_Black_Relax()
   { 
     int Alt_Base=(/*ghost_this*/I.getBase)() - 1; 

     Index Black_Odd(Alt_Base + 1,(/*ghost_this*/Grid_Size - 1) / 2,2); 
     Index Black_Even(Alt_Base + 2,(/*ghost_this*/Grid_Size - 2) / 2,2); 

     Index Red_Odd(Alt_Base + 1,(/*ghost_this*/Grid_Size - 1) / 2,2); 
     Index Red_Even(Alt_Base + 2,(/*ghost_this*/Grid_Size - 2) / 2,2); 

     Index Odd_Rows(Alt_Base + 1,(/*ghost_this*/Grid_Size - 1) / 2,2); 
     Index Even_Rows(Alt_Base + 2,(/*ghost_this*/Grid_Size - 2) / 2,2); 

     if (PDE::Problem_Dimension == 1)
        { 
          /*ghost_this*/Three_Point_Stencil(Black_Even); 
          /*ghost_this*/Three_Point_Stencil(Black_Odd); 
     } 


     else if (PDE::Problem_Dimension == 2)
             { 
               if (PDE::Fully_Variational)
                  { 
                    /*ghost_this*/Nine_Point_Stencil(Black_Odd,Odd_Rows); 
                    /*ghost_this*/Nine_Point_Stencil(Black_Even,Even_Rows); 
                    /*ghost_this*/Nine_Point_Stencil(Red_Even,Odd_Rows); 
                    /*ghost_this*/Nine_Point_Stencil(Red_Odd,Even_Rows); 
               } 
               else 
                  { 
                    /*ghost_this*/Five_Point_Stencil(Black_Odd,Odd_Rows); 
                    /*ghost_this*/Five_Point_Stencil(Black_Even,Even_Rows); 
                    /*ghost_this*/Five_Point_Stencil(Red_Even,Odd_Rows); 
                    /*ghost_this*/Five_Point_Stencil(Red_Odd,Even_Rows); 
               } 
     } 


          else if (PDE::Problem_Dimension == 3)
                  { 
                    if (PDE::Fully_Variational)
                       { 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Black_Odd,Odd_Rows,
                         Odd_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Black_Even,Even_Rows,
                         Odd_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Black_Odd,Odd_Rows,
                         Even_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Black_Even,Even_Rows,
                         Even_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Red_Even,Odd_Rows,Odd_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Red_Odd,Even_Rows,Odd_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Red_Even,Odd_Rows,Even_Rows); 
                         /*ghost_this*/Twenty_Seven_Point_Stencil(Red_Odd,Even_Rows,Even_Rows); 
                    } 
                    else 
                       { 
                         /*ghost_this*/Seven_Point_Stencil(Black_Odd,Odd_Rows,Odd_Rows); 
                         /*ghost_this*/Seven_Point_Stencil(Black_Even,Even_Rows,Odd_Rows); 
                         /*ghost_this*/Seven_Point_Stencil(Black_Odd,Odd_Rows,Even_Rows); 
                         /*ghost_this*/Seven_Point_Stencil(Black_Even,Even_Rows,Even_Rows); 

                         /*ghost_this*/Seven_Point_Stencil(Red_Even,Odd_Rows,Odd_Rows); 
                         /*ghost_this*/Seven_Point_Stencil(Red_Odd,Even_Rows,Odd_Rows); 
                         /*ghost_this*/Seven_Point_Stencil(Red_Even,Odd_Rows,Even_Rows); 
                         /*ghost_this*/Seven_Point_Stencil(Red_Odd,Even_Rows,Even_Rows); 
                    } 
          } 
} 



void PDE::Solve()
   { 
   
// This is the Solve function for this object (every object pretending to represent a solver 

   
// should have one (the MG objects solver calls this one).  Thus details of the 

   
// descritization of the MG problem are isolated (and the complexities contained) in the PDE

   
// object where the specific application problem (together with the descritization of it)

   
// is hidden.  In this way the MG solve is relatively independent of the specifics of the

   
// particular application.


   
// We choose to use the Red_Black relaxation here!

   
// Red_Black_Relax ();


     if (FALSE)
        { 
          /*ghost_this*/Right_Hand_Side.display("RHS -- BEFORE RELAX"); 
          /*ghost_this*/Solution.display("SOLUTION -- BEFORE RELAX"); 
     } 

          
// Jacobi_Relax ();

     /*ghost_this*/Red_Black_Relax(); 


#ifdef GRAPHICS








#endif


     if (FALSE)
          { /*ghost_this*/Solution.display("SOLUTION -- AFTER RELAX"); } 
} 

void PDE::Error()
   { 
   
// This error function computes the error and then prints a few norms of it!


     if (FALSE)
          { printf("Inside of PDE::Error() \n"); } 

     doubleArray Error(fabs(/*ghost_this*/Solution(/*ghost_this*/I,/*ghost_this*/J,/*ghost_this*/K)
     -/*ghost_this*/Exact_Solution(/*ghost_this*/I,/*ghost_this*/J,/*ghost_this*/K))); 
}      
// printf ("L2_Norm Error = %e Max Error = %e Min Error = %e \n", L2_Norm (Error) , max(Error) , min(Error) );


void PDE::Compute_Residuals()
   { 
   
// The decision to implement the residual computation seperately is a design preference.


     if (FALSE)
          { printf("Compute Residuals! \n"); } 

     /*ghost_this*/Residual=0.0; 

     double Scaling_Factor=1.0 / (/*ghost_this*/Mesh_Size * /*ghost_this*/Mesh_Size); 
     if (PDE::Problem_Dimension == 1)
        { 
          /*ghost_this*/Residual(/*ghost_this*/I)=/*ghost_this*/Right_Hand_Side(/*ghost_this*/I)
          +Scaling_Factor*((/*ghost_this*/Solution((/*ghost_this*/I+1))+/*ghost_this*/Solution((/*ghost_this*/I
          -1)))-2.0*/*ghost_this*/Solution(/*ghost_this*/I)); 
     } 

     else if (PDE::Problem_Dimension == 2)
             { 
               if (PDE::Fully_Variational)
                  { 
                    /*ghost_this*/Residual(/*ghost_this*/I,/*ghost_this*/J)=/*ghost_this*/Right_Hand_Side(/*ghost_this*/I,
                    /*ghost_this*/J)+Scaling_Factor*((((((((/*ghost_this*/Solution((/*ghost_this*/I
                    +1),/*ghost_this*/J)+/*ghost_this*/Solution((/*ghost_this*/I-1),/*ghost_this*/J))
                    +/*ghost_this*/Solution(/*ghost_this*/I,(/*ghost_this*/J-1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                    (/*ghost_this*/J+1)))+/*ghost_this*/Solution((/*ghost_this*/I+1),
                    (/*ghost_this*/J+1)))+/*ghost_this*/Solution((/*ghost_this*/I-1),
                    (/*ghost_this*/J+1)))+/*ghost_this*/Solution((/*ghost_this*/I+1),
                    (/*ghost_this*/J-1)))+/*ghost_this*/Solution((/*ghost_this*/I-1),
                    (/*ghost_this*/J-1)))-8.0*/*ghost_this*/Solution(/*ghost_this*/I,
                    /*ghost_this*/J)); 
               } 
               else 
                  { 
                    /*ghost_this*/Residual(/*ghost_this*/I,/*ghost_this*/J)=/*ghost_this*/Right_Hand_Side(/*ghost_this*/I,
                    /*ghost_this*/J)+Scaling_Factor*((((/*ghost_this*/Solution((/*ghost_this*/I
                    +1),/*ghost_this*/J)+/*ghost_this*/Solution((/*ghost_this*/I-1),/*ghost_this*/J))
                    +/*ghost_this*/Solution(/*ghost_this*/I,(/*ghost_this*/J-1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                    (/*ghost_this*/J+1)))-4.0*/*ghost_this*/Solution(/*ghost_this*/I,
                    /*ghost_this*/J)); 
               } 
     } 

#ifdef GRAPHICS






#endif






          else if (PDE::Problem_Dimension == 3)
                  { 
                    if (PDE::Fully_Variational)
                       { 
                         /*ghost_this*/Residual(/*ghost_this*/I,/*ghost_this*/J,/*ghost_this*/K)
                         =/*ghost_this*/Right_Hand_Side(/*ghost_this*/I,/*ghost_this*/J,
                         /*ghost_this*/K)+Scaling_Factor*((((((((((((((((((((((((((/*ghost_this*/Solution((/*ghost_this*/I
                         +1),/*ghost_this*/J,/*ghost_this*/K)+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),/*ghost_this*/J,/*ghost_this*/K))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J-1),/*ghost_this*/K))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J+1),/*ghost_this*/K))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),(/*ghost_this*/J+1),/*ghost_this*/K))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),(/*ghost_this*/J+1),/*ghost_this*/K))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),(/*ghost_this*/J-1),/*ghost_this*/K))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),(/*ghost_this*/J-1),/*ghost_this*/K))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),/*ghost_this*/J,(/*ghost_this*/K-1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),/*ghost_this*/J,(/*ghost_this*/K-1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J-1),(/*ghost_this*/K-1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J+1),(/*ghost_this*/K-1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),(/*ghost_this*/J+1),(/*ghost_this*/K-1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),(/*ghost_this*/J+1),(/*ghost_this*/K-1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),(/*ghost_this*/J-1),(/*ghost_this*/K-1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),(/*ghost_this*/J-1),(/*ghost_this*/K-1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),/*ghost_this*/J,(/*ghost_this*/K+1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),/*ghost_this*/J,(/*ghost_this*/K+1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J-1),(/*ghost_this*/K+1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J+1),(/*ghost_this*/K+1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),(/*ghost_this*/J+1),(/*ghost_this*/K+1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),(/*ghost_this*/J+1),(/*ghost_this*/K+1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         +1),(/*ghost_this*/J-1),(/*ghost_this*/K+1)))+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),(/*ghost_this*/J-1),(/*ghost_this*/K+1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         /*ghost_this*/J,(/*ghost_this*/K-1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         /*ghost_this*/J,(/*ghost_this*/K+1)))-26.0*/*ghost_this*/Solution(/*ghost_this*/I,
                         /*ghost_this*/J,/*ghost_this*/K)); 
                    } 
                    else                     
// Center face





                    
// Inner face





                    
// Outer face





                    
// Center point of Inner and Outer faces


                    
// Center point

                       { 
                         /*ghost_this*/Residual(/*ghost_this*/I,/*ghost_this*/J,/*ghost_this*/K)
                         =/*ghost_this*/Right_Hand_Side(/*ghost_this*/I,/*ghost_this*/J,
                         /*ghost_this*/K)+Scaling_Factor*((((((/*ghost_this*/Solution((/*ghost_this*/I
                         +1),/*ghost_this*/J,/*ghost_this*/K)+/*ghost_this*/Solution((/*ghost_this*/I
                         -1),/*ghost_this*/J,/*ghost_this*/K))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J-1),/*ghost_this*/K))+/*ghost_this*/Solution(/*ghost_this*/I,
                         (/*ghost_this*/J+1),/*ghost_this*/K))+/*ghost_this*/Solution(/*ghost_this*/I,
                         /*ghost_this*/J,(/*ghost_this*/K-1)))+/*ghost_this*/Solution(/*ghost_this*/I,
                         /*ghost_this*/J,(/*ghost_this*/K+1)))-6.0*/*ghost_this*/Solution(/*ghost_this*/I,
                         /*ghost_this*/J,/*ghost_this*/K)); 
                    } 
          } 
     if (FALSE)
          { printf("Leaving Compute Residuals! \n"); } 
} 
          
// Swap sign for test!

          
// Residual = -Residual; 



double PDE::L2_Norm(doubleArray & Array)
   { 
   
// This is the most natural AND most efficient implementation using the array class!


     if (FALSE)
          { printf("Inside of PDE::L2_Norm! \n"); } 

     return sqrt(sum(Array*Array));
} 

void PDE::Three_Point_Stencil(Index & i)
   { 
     if (FALSE)
          { printf("Compute Three_Point_Stencil! \n"); } 

     /*ghost_this*/Solution(i)=(((/*ghost_this*/Mesh_Size * /*ghost_this*/Mesh_Size)*/*ghost_this*/Right_Hand_Side(i)
     +/*ghost_this*/Solution((i+1)))+/*ghost_this*/Solution((i-1)))/2.0; 
     if (FALSE)
          { printf("Leaving Three_Point_Stencil! \n"); } 
} 

void PDE::Five_Point_Stencil(Index & i,Index & j)
   { 
     /*ghost_this*/Solution(i,j)=(((((/*ghost_this*/Mesh_Size * /*ghost_this*/Mesh_Size)
     */*ghost_this*/Right_Hand_Side(i,j)+/*ghost_this*/Solution((i+1),j))+/*ghost_this*/Solution((i
     -1),j))+/*ghost_this*/Solution(i,(j+1)))+/*ghost_this*/Solution(i,(j-1)))/4.0; 
} 
void PDE::Nine_Point_Stencil(Index & i,Index & j)
   { 
     /*ghost_this*/Solution(i,j)=(((((((((/*ghost_this*/Mesh_Size * /*ghost_this*/Mesh_Size)
     */*ghost_this*/Right_Hand_Side(i,j)+/*ghost_this*/Solution((i+1),j))+/*ghost_this*/Solution((i
     -1),j))+/*ghost_this*/Solution(i,(j+1)))+/*ghost_this*/Solution(i,(j-1)))+/*ghost_this*/Solution((i
     +1),(j+1)))+/*ghost_this*/Solution((i+1),(j-1)))+/*ghost_this*/Solution((i-1),(j
     +1)))+/*ghost_this*/Solution((i-1),(j-1)))/8.0; 
} 
void PDE::Seven_Point_Stencil(Index & i,Index & j,Index & k)
   { 
     /*ghost_this*/Solution(i,j,k)=(((((((/*ghost_this*/Mesh_Size * /*ghost_this*/Mesh_Size)
     */*ghost_this*/Right_Hand_Side(i,j,k)+/*ghost_this*/Solution((i+1),j,k))+/*ghost_this*/Solution((i
     -1),j,k))+/*ghost_this*/Solution(i,(j+1),k))+/*ghost_this*/Solution(i,(j-1),k))+/*ghost_this*/Solution(i,
     j,(k+1)))+/*ghost_this*/Solution(i,j,(k-1)))/6.0; 
} 
void PDE::Twenty_Seven_Point_Stencil(Index & i,Index & j,Index & k)
   { 
     /*ghost_this*/Solution(i,j,k)=(((((((((((((((((((((((((((/*ghost_this*/Mesh_Size
      * /*ghost_this*/Mesh_Size)*/*ghost_this*/Right_Hand_Side(i,j,k)+/*ghost_this*/Solution((i
     +1),j,(k-1)))+/*ghost_this*/Solution((i-1),j,(k-1)))+/*ghost_this*/Solution(i,(j
     +1),(k-1)))+/*ghost_this*/Solution(i,(j-1),(k-1)))+/*ghost_this*/Solution((i+1),
     (j+1),(k-1)))+/*ghost_this*/Solution((i+1),(j-1),(k-1)))+/*ghost_this*/Solution((i
     -1),(j+1),(k-1)))+/*ghost_this*/Solution((i-1),(j-1),(k-1)))+/*ghost_this*/Solution((i
     +1),j,k))+/*ghost_this*/Solution((i-1),j,k))+/*ghost_this*/Solution(i,(j+1),k))+/*ghost_this*/Solution(i,
     (j-1),k))+/*ghost_this*/Solution((i+1),(j+1),k))+/*ghost_this*/Solution((i+1),(j
     -1),k))+/*ghost_this*/Solution((i-1),(j+1),k))+/*ghost_this*/Solution((i-1),(j-1),
     k))+/*ghost_this*/Solution((i+1),j,(k+1)))+/*ghost_this*/Solution((i-1),j,(k+1)))
     +/*ghost_this*/Solution(i,(j+1),(k+1)))+/*ghost_this*/Solution(i,(j-1),(k+1)))+/*ghost_this*/Solution((i
     +1),(j+1),(k+1)))+/*ghost_this*/Solution((i+1),(j-1),(k+1)))+/*ghost_this*/Solution((i
     -1),(j+1),(k+1)))+/*ghost_this*/Solution((i-1),(j-1),(k+1)))+/*ghost_this*/Solution(i,
     j,(k-1)))+/*ghost_this*/Solution(i,j,(k+1)))/26.0; 
}      
// Center point of Inner and Outer faces

 PDE & PDE::operator=(const PDE & X)
   { 
   
// This function takes an existing object and copies.

   
// It is ALWAYS a good idea to define both this and the copy constructor,

   
//  even if they are not implemented (though we implement both in this code).


     /*ghost_this*/I=X.I; 
     /*ghost_this*/J=X.J; 
     /*ghost_this*/K=X.K; 

     /*ghost_this*/Solution=X.Solution; 
     /*ghost_this*/Exact_Solution=X.Exact_Solution; 
     /*ghost_this*/Residual=X.Residual; 
     /*ghost_this*/Right_Hand_Side=X.Right_Hand_Side; 

     /*ghost_this*/Mesh_Size = X.Mesh_Size; 
     /*ghost_this*/Grid_Size = X.Grid_Size; 

     return *this;
 } 
