
// Example from Andreas, showing use of struct within a variable declaration
// "struct {} x;" might be a simpler example. But the example from Andreas is
// typical of what appears in the EDG AST.
struct var21
   {
     struct iceland 
        {
          int x;
        } var1;
   };

// Simpler forms of how islands are formed in the AST
struct foo_struct
   {
     int x;
   } ABC3;

class foo_class
   {
     int x;
   } ABC4;

union foo_union
   {
     int x;
   } ABC2;

// More complex ways in which islands are formed within typedef declarations
typedef struct tag_struct 
   {
     int x;
   } ABC5;

// This is a current bug (see test2004_17.C)
typedef class tag_class
   {
     int x;
   } ABC6;

typedef union tag_union 
   {
     int x;
   } ABC7;


