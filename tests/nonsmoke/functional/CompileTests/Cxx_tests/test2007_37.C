// Bug from Robert Preissl

namespace A
   {
     namespace B
        {
          int b;
        };
   };

void
foo()
   {
  // This using declaration does not reference the SgInitializedName IR node
  // but does reference the SgVariableDeclaration (OK, but not very well done).
  // This should be fixed to reference the SgInitializedName directly for the 
  // case of a variable and of course the function declaration for a function.
     using A::B::b;

     b = 0;
   }
