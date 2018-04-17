
class PDE // Partial_Differential_Equations for Iterative Methods on Square Domains
   {
     public:    // Visible to everyone who uses this class

       // Indexes for computing any stencil applied uniformally to interior.

          Index I,J,K;

          doubleArray Solution;
          doubleArray Exact_Solution;
          doubleArray Residual;
          doubleArray Right_Hand_Side;

          double Mesh_Size;
          int Grid_Size;
          static int Problem_Dimension;
          static Boolean Fully_Variational;

       // **********  Public member functions ************

          PDE ();
         ~PDE ();

          PDE ( int Local_Grid_Size );
 
          PDE ( int Local_Grid_Size ,
                double (* Function_RHS) ( double X , double Y , double Z ) );
 
          PDE ( int Local_Grid_Size ,
                double (* Function_RHS)            ( double X , double Y , double Z ) ,
                double (* Exact_Solution_Function) ( double X , double Y , double Z ) );

          void Construct_PDE ( int Grid_Size );
          void Initialize_PDE ( double (* Function_RHS ) ( double X , double Y , double Z ) );

       // faster function that the previous point evaluation (better for P++ as a result)
          void Setup_RHS ();
          void Setup_Exact_Solution ();

          void Setup_PDE_Exact_Solution ( double (* Exact_Solution_Function ) ( double X , double Y , double Z ) );

          void Initialize_Array_To_Function (
                    doubleArray & Array , double (* Primative_Function) ( double X , double Y , double Z ) );

          double L2_Norm ( doubleArray & Array );

       // Member Functions to Manipulate Data
          void Jacobi_Relax ();
          void Red_Black_Relax ();

          void Solve ();
          void Error ();
          void Compute_Residuals ();

          PDE & operator= ( const PDE & Rhs ); // takes an existing object and copies.

     protected: // Visible to member and any class derived from this class

     private:   // Visible only to members

          PDE::PDE ( const PDE & );       // makes an object from a like object.

       // A stencil class could be used here!
       // 1D operator
          void Three_Point_Stencil        ( Index & I );
       // 2D operator
          void Five_Point_Stencil         ( Index & I , Index & J );
          void Nine_Point_Stencil         ( Index & I , Index & J );
       // 3D operator
          void Seven_Point_Stencil        ( Index & I , Index & J , Index & K );
          void Twenty_Seven_Point_Stencil ( Index & I , Index & J , Index & K );
   };


