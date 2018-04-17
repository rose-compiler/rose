template < class T, int Template_Dimension > class Array_Descriptor_Type;
//         ***********  C++ Single Level MG CODE  ************



#include "rose_mg_main.h"



#define DEBUG 0



// ***********************************************************************


//                     SINGLE LEVEL MULTIGRID C++ CLASS


// ***********************************************************************
Single_MG_Level::Single_MG_Level()
   { 
   
// Not much to do since the PDE constructor was already called first!

     if (FALSE)
          { printf("Single_MG_Level CONSTRUCTOR called \n"); } 
} 


Single_MG_Level::~Single_MG_Level()
   { 
     if (FALSE)
          { printf("Single_MG_Level DESTRUCTOR called \n"); } 
} 

 Single_MG_Level & Single_MG_Level::operator=(const Single_MG_Level & Rhs)
   { 
   
// takes an existing object and copies.


     PDE::operator=(Rhs); 

     return *this;
 } 

void Single_MG_Level::Interpolate(doubleArray & Grid,doubleArray & Coarser_Grid,int Order_Of_Operator)


   { 
     if (FALSE)
          { printf("Inside of Single_MG_Level::Interpolate \n"); } 

     if (Order_Of_Operator != 2)
        { 
          printf("ERROR: Only Order_Of_Operator = 2 supported in Single_MG_Level::Interpolate! \n"); 
     } 

     int Alt_Base=(/*ghost_this*/I.getBase)() - 1; 

     
// Index objects for fine grid!

     



     
// Index objects for coarse grid!













     
// These are not needed if using RB relaxation with 3-point operator.










     
// These are not needed if using RB relaxation with 5-point operator.




















     
// These are not needed if using RB relaxation with 5-point operator.












































     
// APP_DEBUG = 6;


#if 1




#endif

     
// Coarser_Grid (Coarser_Grid_Index_I,Coarser_Grid_Index_J,Coarser_Grid_Index_K);

     
// Coarser_Grid (Coarser_Grid_Index_I);

     
// Grid (Grid_Index_I,Grid_Index_J,Grid_Index_K);

     
// Grid (Grid_Index_I);


     
// printf ("Exiting in Single_MG_Level::Projection \n");

     
// APP_ABORT();










     
// Coarser_Grid (Coarser_Grid_Index_I) *= 2.0;





























     
// K-1 plane!











     
// K+1 plane!
Index I_F(Alt_Base + 0,(/*ghost_this*/Grid_Size - 2) / 2 + 1,2); 
     Index J_F(I_F); 
     Index K_F(I_F); 
     Index I_C(Alt_Base + 0,(/*ghost_this*/Grid_Size - 2) / 2 + 1,1); 
     Index J_C(I_C); 
     Index K_C(I_C); 
     if (
     FALSE)
        { 
          Grid.display("Grid (BEFORE INTERPOLATION)"); 
     } 
     if (
     PDE::Problem_Dimension == 1)
        { 
          Grid((I_F+1))=(Coarser_Grid(I_C)+Coarser_Grid((I_C+1)))/2.0; 
          Grid(I_F)=Coarser_Grid(I_C); 
     } 
     else if (
          PDE::Problem_Dimension == 2)
             { 
               Grid((I_F+1),J_F)=(Coarser_Grid(I_C,J_C)+Coarser_Grid((I_C+1),J_C))/2.0; 
               Grid(I_F,(J_F+1))=(Coarser_Grid(I_C,J_C)+Coarser_Grid(I_C,(J_C+1)))/2.0; 
               Grid(I_F,J_F)=Coarser_Grid(I_C,J_C); 
               Grid((I_F+1),(J_F+1))=(((Coarser_Grid(I_C,J_C)+Coarser_Grid(I_C,(J_C+1)))
               +Coarser_Grid((I_C+1),J_C))+Coarser_Grid((I_C+1),(J_C+1)))/4.0; 
     } 
          else if (
               PDE::Problem_Dimension == 3)
                  { 
                    Grid((I_F+1),J_F,K_F)=(Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid((I_C
                    +1),J_C,K_C))/2.0; 
                    Grid(I_F,(J_F+1),K_F)=(Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid(I_C,
                    (J_C+1),K_C))/2.0; 
                    Grid(I_F,J_F,(K_F+1))=(Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid(I_C,
                    J_C,(K_C+1)))/2.0; 
                    Grid((I_F+1),J_F,(K_F+1))=(((Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid((I_C
                    +1),J_C,K_C))+Coarser_Grid(I_C,J_C,(K_C+1)))+Coarser_Grid((I_C+1),
                    J_C,(K_C+1)))/4.0; 
                    Grid(I_F,(J_F+1),(K_F+1))=(((Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid(I_C,
                    (J_C+1),K_C))+Coarser_Grid(I_C,J_C,(K_C+1)))+Coarser_Grid(I_C,(J_C
                    +1),(K_C+1)))/4.0; 
                    Grid((I_F+1),(J_F+1),K_F)=(((Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid(I_C,
                    (J_C+1),K_C))+Coarser_Grid((I_C+1),J_C,K_C))+Coarser_Grid((I_C+1),
                    (J_C+1),K_C))/4.0; 
                    Grid(I_F,J_F,K_F)=Coarser_Grid(I_C,J_C,K_C); 
                    Grid((I_F+1),(J_F+1),(K_F+1))=(((((((Coarser_Grid(I_C,J_C,K_C)+Coarser_Grid(I_C,
                    (J_C+1),K_C))+Coarser_Grid((I_C+1),J_C,K_C))+Coarser_Grid((I_C+1),
                    (J_C+1),K_C))+Coarser_Grid(I_C,J_C,(K_C+1)))+Coarser_Grid(I_C,(J_C
                    +1),(K_C+1)))+Coarser_Grid((I_C+1),J_C,(K_C+1)))+Coarser_Grid((I_C
                    +1),(J_C+1),(K_C+1)))/8.0; 
          } 
     if (
     FALSE)
        { 
          Coarser_Grid.display("Coarser_Grid"); 
          Grid.display("Grid"); 
     } 
     if (
     FALSE)
          { 
          printf("Leaving Single_MG_Level::Interpolate \n"); } 
} 
void Single_MG_Level::Projection(doubleArray & Grid,doubleArray & Coarser_Grid,int Order_Of_Operator)
   { 
     int Alt_Base=(/*ghost_this*/I.getBase)() - 1; 
     Index Grid_Index_I(Alt_Base + 2,(/*ghost_this*/Grid_Size - 2) / 2,2); 
     Index Grid_Index_J(((PDE::Problem_Dimension >= 2)?Grid_Index_I:Index())); 
     Index Grid_Index_K(((PDE::Problem_Dimension == 3)?Grid_Index_I:Index())); 
     Index Coarser_Grid_Index_I(Alt_Base + 1,(/*ghost_this*/Grid_Size - 2) / 2,1); 
     Index Coarser_Grid_Index_J(((PDE::Problem_Dimension >= 2)?Coarser_Grid_Index_I:Index())); 
     Index Coarser_Grid_Index_K(((PDE::Problem_Dimension == 3)?Coarser_Grid_Index_I:Index())); 
     if (
     FALSE)
          { 
          printf("Inside of Single_MG_Level::Projection  --  Order_Of_Operator = %d \n",
          Order_Of_Operator); } 
     if (
     !((Order_Of_Operator == 1) || (Order_Of_Operator == 2)))
        { 
          printf("ERROR: Only Order_Of_Operator = 1 || 2 supported in Single_MG_Level::Projection! \n"); 
     } 
     if (
     Order_Of_Operator == 1)
        { 
          Coarser_Grid(Coarser_Grid_Index_I,Coarser_Grid_Index_J,Coarser_Grid_Index_K)
          =Grid(Grid_Index_I,Grid_Index_J,Grid_Index_K); 
     } 
     if (
     Order_Of_Operator == 2)
        { 
          if (
          PDE::Problem_Dimension == 1)
             { 
               Coarser_Grid(Coarser_Grid_Index_I)=Grid(Grid_Index_I)+0.5*(Grid((Grid_Index_I
               -1))+Grid((Grid_Index_I+1))); 
               Coarser_Grid(Coarser_Grid_Index_I)*=0.5; 
          } 
          else if (
               PDE::Problem_Dimension == 2)
                  { 
                    Coarser_Grid(Coarser_Grid_Index_I,Coarser_Grid_Index_J)=(Grid(Grid_Index_I,
                    Grid_Index_J)+0.5*(((Grid(Grid_Index_I,(Grid_Index_J-1))+Grid(Grid_Index_I,
                    (Grid_Index_J+1)))+Grid((Grid_Index_I-1),Grid_Index_J))+Grid((Grid_Index_I
                    +1),Grid_Index_J)))+0.25*(((Grid((Grid_Index_I-1),(Grid_Index_J
                    -1))+Grid((Grid_Index_I+1),(Grid_Index_J-1)))+Grid((Grid_Index_I-1),
                    (Grid_Index_J+1)))+Grid((Grid_Index_I+1),(Grid_Index_J+1))); 
                    Coarser_Grid(Coarser_Grid_Index_I,Coarser_Grid_Index_J)*=0.25; 
          } 
               else if (
                    PDE::Problem_Dimension == 3)
                       { 
                         Coarser_Grid(Coarser_Grid_Index_I,Coarser_Grid_Index_J,Coarser_Grid_Index_K)
                         =((Grid(Grid_Index_I,Grid_Index_J,Grid_Index_K)+0.5*(((Grid(Grid_Index_I,
                         (Grid_Index_J-1),Grid_Index_K)+Grid(Grid_Index_I,(Grid_Index_J
                         +1),Grid_Index_K))+Grid((Grid_Index_I-1),Grid_Index_J,Grid_Index_K))
                         +Grid((Grid_Index_I+1),Grid_Index_J,Grid_Index_K)))+0.25
                         *(((Grid((Grid_Index_I-1),(Grid_Index_J-1),Grid_Index_K)+Grid((Grid_Index_I
                         +1),(Grid_Index_J-1),Grid_Index_K))+Grid((Grid_Index_I-1),(Grid_Index_J
                         +1),Grid_Index_K))+Grid((Grid_Index_I+1),(Grid_Index_J+1),Grid_Index_K)))
                         +0.5*(((((Grid(Grid_Index_I,Grid_Index_J,(Grid_Index_K-1))
                         +0.5*(((Grid(Grid_Index_I,(Grid_Index_J-1),(Grid_Index_K
                         -1))+Grid(Grid_Index_I,(Grid_Index_J+1),(Grid_Index_K-1)))+Grid((Grid_Index_I
                         -1),Grid_Index_J,(Grid_Index_K-1)))+Grid((Grid_Index_I+1),Grid_Index_J,
                         (Grid_Index_K-1))))+0.25*(((Grid((Grid_Index_I-1),(Grid_Index_J
                         -1),(Grid_Index_K-1))+Grid((Grid_Index_I+1),(Grid_Index_J-1),
                         (Grid_Index_K-1)))+Grid((Grid_Index_I-1),(Grid_Index_J+1),(Grid_Index_K
                         -1)))+Grid((Grid_Index_I+1),(Grid_Index_J+1),(Grid_Index_K-1))))
                         +Grid(Grid_Index_I,Grid_Index_J,(Grid_Index_K+1)))+0.5*(((Grid(Grid_Index_I,
                         (Grid_Index_J-1),(Grid_Index_K+1))+Grid(Grid_Index_I,(Grid_Index_J
                         +1),(Grid_Index_K+1)))+Grid((Grid_Index_I-1),Grid_Index_J,(Grid_Index_K
                         +1)))+Grid((Grid_Index_I+1),Grid_Index_J,(Grid_Index_K+1))))
                         +0.25*(((Grid((Grid_Index_I-1),(Grid_Index_J-1),(Grid_Index_K
                         +1))+Grid((Grid_Index_I+1),(Grid_Index_J-1),(Grid_Index_K+1)))
                         +Grid((Grid_Index_I-1),(Grid_Index_J+1),(Grid_Index_K+1)))+Grid((Grid_Index_I
                         +1),(Grid_Index_J+1),(Grid_Index_K+1)))); 








                         Coarser_Grid(Coarser_Grid_Index_I,Coarser_Grid_Index_J,Coarser_Grid_Index_K)
                         *=0.125; 
               } 
     } 


     if (FALSE)
        { 
          Coarser_Grid.display("Coarser_Grid"); 
          Grid.display("Grid"); 
     } 

     if (FALSE)
          { printf("Leaving Single_MG_Level::Projection \n"); } 
} 
