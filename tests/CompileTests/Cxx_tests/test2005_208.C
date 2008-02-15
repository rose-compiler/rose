class X
   {
     public:
       // note qualified name X::X_int (which is a type alias) does not conflict 
       // with ::X_int (which is a variable) it would not be a problem if they 
       // were both type aliases or both variable names.
          typedef X X_int;

          class X* X1;
          class ::X* X2;
   };

// If this is uncommented then we get a SgTemplateInstantiation with two parents
// The solution is to not share the declaration in the typedef with the declaration for this 
// variable declaration!  In this case there would be perhaps two non-defining declarations
// (likely only one) but still only one defining declaration.  All would reference the
// single definition which has only the defining declaration as an associated declaration.
class ::X X_int;

X X_char;
X X_long;
