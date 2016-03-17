// Since this will be copied to the generated_dsl_attributes.C 
// file, we want to include the generated_dsl_attributes.h, not 
// the nongenerated_dsl_attributes.h file.
// #include "generated_dsl_attributes.h"

#include "dsl_attribute_support.h"

// We need the headers representing the abstractions for the DSL.
#include "array.h"

#include "nongenerated_dsl_attributes.h"

#if 0
// The number of DSL types to define is know at compile time and generated as
// part of the DSL attribute code generation.
std::vector<std::string> dsl_type_names = { "array" };

// Reference function names that are specific to the DSL.
std::vector<std::string> dsl_function_names = { "operator+" };

// Reference member functions (class and member function names) that are specific to the DSL.
std::vector< std::pair<std::string,std::string> > dsl_member_function_names = { {"array","zero"} };

// Build a vector of map of attributes (ultimately we want a map).
// std::vector<dsl_attribute*> dsl_attribute_list = { new array_dsl_attribute() };
// std::vector<std::pair<std::string,dsl_attribute*> > dsl_attribute_list = { {"array", new array_dsl_attribute()} };
// std::vector<std::pair<std::string,dsl_attribute> > dsl_attribute_list = { {"array", array_dsl_attribute()} };
std::map<std::string,dsl_attribute> dsl_attribute_map = { {"array", array_dsl_attribute()} };
#endif


