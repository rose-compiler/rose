// ***** Multigrid class for processing a single level using PDE class  *****

class Single_MG_Level : public PDE 
   {
  // This class exteends the previous class (PDE) so that it will include
  // those operations (operators) that MG will need for each level.

     public:    // Visible to everyone who uses this class

          Single_MG_Level ();
         ~Single_MG_Level ();

          void Projection  ( doubleArray & Grid ,  
                             doubleArray & Coarser_Grid ,
                             int Order_Of_Operator = 2 );

          void Interpolate ( doubleArray & Grid ,
                             doubleArray & Coarser_Grid ,
                             int Order_Of_Operator = 2 );

       // Takes an existing object and copies.
          Single_MG_Level & operator= ( const Single_MG_Level & Rhs ); 

     protected: // Visible to members and any class derived from this class

     private:   // Visible only to members

          Single_MG_Level (  const Single_MG_Level & ); // makes an object from a like object.
   };

