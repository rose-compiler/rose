
// DQ (7/23/2020): GNU 10 STL requires that we include the STL string header file explicitly.
#include <string>

#include <vector>
#include <map>



#pragma DSL keyword
class array
   {
     public:
       // int* data;

       // Default constructor
          array();

       // Copy constructor
       // array(const array & X);

       // Assignment operator
          array operator= (const array & X);

       // Simple array arithmetic operators
       // friend array operator* (const array & X, const array & Y);
       // friend array operator/ (const array & X, const array & Y);
          friend array operator+ (const array & X, const array & Y);
       // friend array operator- (const array & X, const array & Y);

       // returns array of size set to zeros.
       // array zero(int size);

       // returns array of size set to ones.
       // array one (int size);
   };


// Reference classe names that are specific to the DSL.
extern std::vector<std::string> dsl_type_names;

// Reference functions names that are specific to the DSL.
extern std::vector<std::string> dsl_function_names;

// Reference member functions (class and member function names) that are specific to the DSL.
extern std::vector< std::pair<std::string,std::string> > dsl_member_function_names;

class AstAttribute {};

// Example attributes that we need to have generated to support array DSL example.
class dsl_attribute : public AstAttribute
   {
     public:
       // array value;
   };

// References to dsl attributes in a map inexed by the name of the dsl abstraction.
extern std::map<std::string,dsl_attribute> dsl_attribute_map;

// Examples of classes to be generated for to support the DSL compiler.
// NOTE: we might not need to generate this if it could be a template.
class array_dsl_attribute : public dsl_attribute
   {
     public:
       // Depending on the compile time semantics we want to leverage we may not need an object here.
          array value;
   };

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
