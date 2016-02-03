// We need the rose.h so that we can get the defining declaration of AstAttribute
// Which we require to be a base class to all of the generated attributes.
#include "rose.h"

// We need the headers representing the abstractions for the DSL.
#include "array.h"

// Since this will be copied to the generated_dsl_attributes.C 
// file, we want to include the generated_dsl_attributes.h, not 
// the nongenerated_dsl_attributes.h file.
#include "generated_dsl_attributes.h"

// The number of DSL types to define is know at compile time and generated as
// part of the DSL attribute code generation.
char* dsl_types[1] = 
   {
     "array"
   };


// Reference functions that are specific to the DSL.
char* dsl_functions[1] = { "" };

// Reference member functions (class and member function) that are specific to the DSL.
char* dsl_member_functions[1][2] = { "","" };
