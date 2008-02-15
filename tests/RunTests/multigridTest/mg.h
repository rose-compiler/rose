// ***** Multigrid class for Iterative Methods using PDE class *****

class MG : public PDE 
   {
     public:    // Visible to everyone who uses this class

          static const int FIRST_ORDER_OPERATOR;
          static const int SECOND_ORDER_OPERATOR;

          static Boolean Projection_Operator_Order;
          static Boolean Interpolation_Operator_Order;


          int COARSEST_LEVEL;

          int Max_Multigrid_Level;                 // Coarsest level is value 1!
          Single_MG_Level *const Multigrid_Level;  // Array of grid levels

          int Number_Of_MG_Iterations;
          int Number_Of_Relaxations_Down_V_Cycle;
          int Number_Of_Relaxations_Up_V_Cycle;

       // doubleArray Solution;
       // doubleArray Right_Hand_Side;
       // doubleArray Exact_Solution;
       // doubleArray Residual;

          double Mesh_Size;
          int    Grid_Size;

          Index I,J,K;

       // Functions to define object interface

         ~MG ();
          MG ();

          MG ( int Local_Grid_Size );

          MG ( int Local_Grid_Size , double (* Function_RHS) ( double X , double Y , double Z ) );

          MG ( int Local_Grid_Size ,
               double (* Function_RHS)            ( double X , double Y , double Z ) ,
               double (* Exact_Solution_Function) ( double X , double Y , double Z ) );

          void Construct_MG  ( int Fine_Grid_Size );
          void Initialize_MG ( double (* Function_RHS) ( double X , double Y , double Z ) );

          void Setup_Exact_Solution ( double (* Exact_Solution_Function) ( double X , double Y , double Z ) );

          void Residual_Projection ( int Level );
          void Interpolate_And_Add ( int Level );

          void V_Cycle ( int Level , int Max_Level );
          void Full_MultiGrid_Cycle ();

          void Solve ();

          MG & operator= ( const MG & Rhs ); // takes an existing object and copies.

     protected: // Visible to member and any class derived from this class

     private:   // Visible only to members

          MG ( const MG & );        // makes an object from a like object.
   };




