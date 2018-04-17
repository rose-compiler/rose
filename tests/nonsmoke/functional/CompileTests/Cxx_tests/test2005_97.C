// Example of where multiple SgInitializedName objects are built for the "x" in "int x;"

// Example #1
namespace X
   {
  // In this variable declaration "x" is a SgInitializedName object
     extern int x;
   }

// This is a different SgInitializedName object (but it is a reference to the same variable above)
// DQ (2/7/2006): This variable is being placed into both the global smybol table and the symbol table for namespace X.
int X::x = 0;


// Example #2
class Y
   {
     public:
       // In this variable declaration "x" is a SgInitializedName object
          int x;

       // This is a different SgInitializedName object (but it is a reference to the same variable above)
          Y():x(1) {}      
   };


#if 0
// Unrelated code:
template <typename T> class templatedClass;

template <typename T> class templatedClass
   {
   };

template <typename T> class templatedClass;


// This is the first non-defining declaration
void foo();

// This is a redundant declaration (not the first non-defining declaration)
// However, this declaration will exist in the AST.
void foo();

// This is the defining declaration (different from the first non-defining declaration)
void foo()
   {
  // start of function scope

   {
     int x;
   }

  // empty scope!
   {
   }

   {
     int x;
   }

  // end of function scope
   }

// Example of where first non-defining declaration and defining declaration are the same
void foobar() {};
#endif

