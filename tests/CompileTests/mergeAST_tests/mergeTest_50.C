// This demonstrates where in fixing up the sharedInitializedName in the symbol table, 
// it can have been previous fixed up by a previous reference to the sharedInitializedName.
// So we have to check if the associated shared symbol exists in the symbol table already!

// This was a separate test
// void foobar ();

class X
   {
     public:
       // Bug disappears if constructor with preinitialization list appears before variable declaration!
       // X() : x(1) {}
          int x;

       // Placing the constructor with preinitialization list after the variable declaration demonstrates error
       // symbol table already has an entry for "x" at this point!
          X() : x(1) {}         
   };
