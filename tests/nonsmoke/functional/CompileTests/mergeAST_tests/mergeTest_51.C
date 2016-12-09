// This example deomonstrates how expressions (associated with the initializers in
// member function preinitialization lists) are orphaned within the merge process.
class X
   {
     public:
       // Bug disappears if constructor with preinitialization list appears before variable declaration!
       // The problem is that the member functions are shared (actually the whole class is shared so it doesn't make
       // much difference that the member functions are shared out from under the SgInitializedName objects that 
       // have initializer expressions (which can't be shared currently).  All but the expressions are shared
       // so in the end, the expressions are left undeleted (and orphaned in the AST).  It might be simpler to
       // let the matching depend on the result of the ODR match.
          X() : x(1) {}
          X(int y) : x(2) {}
          int x;
   };
