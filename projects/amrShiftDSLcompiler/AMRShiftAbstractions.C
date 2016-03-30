
// This file is only a C++ file that includes the header file(s) to be supprted by a DSL.  
// The abstractions marked with #pragma DSL keyword are used as a basis for the generation
// of code to represent attributes.  These attributes will be used as part of the DSL 
// compiler.

// General support for attributes (from STL, ROSE, etc.).
#include "dsl_attribute_support.h"

#define DIM 2

// The target DSL abstractions.
// #include "array.h"
// #include "Point.H"
// #include "Stencil.H"
#include "Multigrid.H"

// Use the examples in the nongenerated_dsl_attributes.h to simplify the AST construction for the generated code.
// #include "nongenerated_dsl_attributes.h"

// DQ (2/24/2016): Now I think I want it after!
// DQ (2/24/2016): We want to move this to before the inclusion of the target abstraction.
// It should not be in the "dsl_attribute_support.h" file because these defining declarations
// should appear only once in a single translation unit, and the header file in intended to be 
// included multiple times.

// Include the definitions of the variable with their initializations.
// This is a redundant specification of these variables but with default example initializers

// The number of DSL types to define is know at compile time and generated as
// part of the DSL attribute code generation.
std::vector<std::string> dsl_type_names = { "EXAMPLE" };

// Reference function names that are specific to the DSL.
std::vector<std::string> dsl_function_names = { "EXAMPLE_FUNCTION" };

// Reference member functions (class and member function names) that are specific to the DSL.
std::vector< std::pair<std::string,std::string> > dsl_member_function_names = { {"EXAMPLE_CLASS","EXAMPLE_MEMBER_FUNCTION"} };

// Build a vector of map of attributes (ultimately we want a map).
// std::vector<dsl_attribute*> dsl_attribute_list = { new array_dsl_attribute() };
// std::vector<std::pair<std::string,dsl_attribute*> > dsl_attribute_list = { {"array", new array_dsl_attribute()} };
// std::vector<std::pair<std::string,dsl_attribute> > dsl_attribute_list = { {"array", array_dsl_attribute()} };
std::map<std::string,dsl_attribute> dsl_attribute_map = { {"EXAMPLE_CLASS", dsl_attribute()} };

