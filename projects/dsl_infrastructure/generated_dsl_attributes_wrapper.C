
// I don't appear to turn this on in the compilation of this file with the attribute generator.
// #define SKIP_ROSE_HEADERS

// #include "generated_dsl_attributes.h"
#include "dsl_attribute_support.h"

// DQ (3/6/2016): For the AMRShift work we don't want to include the array.h file.
// This line can maybe be generic in the future, it is here in the case that
// we want to use the DSL abstractions semantics at comile-time to evaluate
// constant expressions.
// #include "array.h"
// #include "Point.H"

// This is the generated code from the attributeGenerator run over the DSL target abstraction.
// #include "generated_dsl_attributes.C"

#if 0
#if 0
class dsl_attribute{};

class array_dsl_attribute : public dsl_attribute
   {
   };
#else
   #include "nongenerated_dsl_attributes.h"
#endif
#endif

// Bug in ROSE where we are unparsing this without the C++11 initializer syntax.
// std::vector< std::string  > dsl_type_names(("EXAMPLE"),("array"));
// std::vector< std::string  > dsl_type_names = { {"EXAMPLE"},{"array"}};

#if 0
std::vector< std::string  > dsl_function_names = { "EXAMPLE_FUNCTION" };
std::vector< std::pair< std::string  , std::string  >  > dsl_member_function_names = { { {"EXAMPLE_CLASS"},{"EXAMPLE_MEMBER_FUNCTION"} } };
// std::map< std::string  , dsl_attribute  > dsl_attribute_map = { { {"EXAMPLE_CLASS"},{dsl_attribute()} },{ {"array"},{array_dsl_attribute()} } };
// std::map< std::string  , dsl_attribute  > dsl_attribute_map = { { {"EXAMPLE_CLASS"},{dsl_attribute()} },{ {"array"},{dsl_attribute()} } };
// std::map< std::string  , dsl_attribute  > dsl_attribute_map = { {"EXAMPLE_CLASS"},{dsl_attribute()} };
// std::map<std::string,dsl_attribute> dsl_attribute_map = { {"array", array_dsl_attribute()} };
// std::map< std::string  , dsl_attribute  > dsl_attribute_map = { {"EXAMPLE_CLASS",dsl_attribute()} };
std::map< std::string  , dsl_attribute  > dsl_attribute_map = { {"EXAMPLE_CLASS",dsl_attribute()} , {"array",array_dsl_attribute()} };
#endif


using namespace std;


dsl_attribute::dsl_attribute() {}

dsl_attribute::~dsl_attribute() {}

string
dsl_attribute::toString()
   {
     return "DSL_Attribute";
   }

string 
dsl_attribute::additionalNodeOptions()
   {
  // Base class additionalNodeOptions() function should be an error to be called.

     printf ("Error: base class function called: needs an implementation on the derived function \n");
#ifndef SKIP_ROSE_HEADERS
     ROSE_ASSERT(false);
#endif
  // return "fillcolor=\"green\",style=filled";
     return "";
   }

string
dsl_attribute::get_name()
   {
     return "DSL-child";
   }

string
dsl_attribute::get_color()
   {
#if 0
  // Base class get_color() function should be an error to be called.

     printf ("Error: base class function called: needs an implementation on the derived function \n");
     ROSE_ASSERT(false);

     return "";
#else
     return "blue";
#endif
   }

vector<AstAttribute::AttributeEdgeInfo>
dsl_attribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     vector<SgNode*>::iterator i = dslChildren.begin();
     while ( i != dslChildren.end() )
        {
#ifndef SKIP_ROSE_HEADERS
          ROSE_ASSERT(currentNode != NULL);
#endif
#if 0
          printf ("Adding an edge from %p = %s to %p = %s \n",currentNode,currentNode->class_name().c_str(),*i,(*i)->class_name().c_str());
#endif
          string name  = get_name();
          string color = get_color();

       // string options = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + color + " ";
          string options = " arrowsize=4.0 style=\"setlinewidth(7)\" constraint=true color=" + color + " ";

#ifndef SKIP_ROSE_HEADERS
          AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) currentNode,*i,name,options);

          v.push_back(additional_edge);
#endif
          i++;
        }

     return v;
   }


#if 0
// ***************************************************************************************************
// Examples of more complex code generation for automated handling of DSL support code to DSL compiler
// ***************************************************************************************************

array_dsl_attribute::array_dsl_attribute() {}
array_dsl_attribute::~array_dsl_attribute() {}

string
array_dsl_attribute::toString()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "array_attribute";
   }

string 
array_dsl_attribute::additionalNodeOptions()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "fillcolor=\"red2\",style=filled";
   }

string
array_refexp_dsl_attribute::toString()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "array_refexp_attribute";
   }

string 
array_refexp_dsl_attribute::additionalNodeOptions()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "fillcolor=\"orange\",style=filled";
   }

string
value_refexp_dsl_attribute::toString()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "array_refexp_attribute";
   }

string 
value_refexp_dsl_attribute::additionalNodeOptions()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "fillcolor=\"turquoise2\",style=filled";
   }

string 
array_assignment_operator_dsl_attribute::toString()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "array_attribute";
   }

string 
array_assignment_operator_dsl_attribute::additionalNodeOptions()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "fillcolor=\"lightgreen\",style=filled";
   }

string
plus_operator_dsl_attribute::toString()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
     return "array_attribute";
   }

string 
plus_operator_dsl_attribute::additionalNodeOptions()
   {
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
  // return "fillcolor=\"aquamarine\",style=filled";
     return "fillcolor=\"green\",style=filled";
   }

#endif
