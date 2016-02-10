// This file is only a C++ file that includes the header file(s) to be supprted by a DSL.  
// The abstractions marked with #pragma DSL keyword are used as a basis for the generation
// of code to represent attributes.  These attributes will be used as part of the DSL 
// compiler.

// General support for attributes (from STL, ROSE, etc.).
#include "dsl_attribute_support.h"

// The target DSL abstractions.
#include "array.h"

// Use the examples in the nongenerated_dsl_attributes.h to simplify the ASC construction for the generated code.
#include "nongenerated_dsl_attributes.h"


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

