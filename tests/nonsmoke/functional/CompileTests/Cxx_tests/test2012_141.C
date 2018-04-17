// Another particularly twisted C++ example
// This must be output as: "typedef struct {void S();}S;" instead of "typedef struct S {void S();}S;"
typedef struct 
   {
  // S(); // error since return type is required to avoid being confused with a constructor
     void S();
} S,T;

#if 1
// In this case we have to output a generated name so that the "var_T"
// can be declared in a seperate declaration.  Later we will fix this
// to use a single declaration for all of the variables in such declaration
// lists.
struct 
   {
     void S();
   } var_S, var_T;
#endif

