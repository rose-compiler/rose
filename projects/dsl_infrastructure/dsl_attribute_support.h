
// We need the rose.h so that we can get the defining declaration of AstAttribute
// Which we require to be a base class to all of the generated attributes.
#ifndef SKIP_ROSE_HEADERS
// We mostly need this to be able to compile this file because of the reference to AstAttribute.
   #include "rose.h"
#else
// Declearations we need to process this file using ROSE in C++11 mode (also simpler then processing "rose.h").
#if 1
#include <string>
#include <vector>
#include <map>
#else
namespace std
   {
     class string { public: string(const char* s); };
     template<typename _Tp> struct vector { public: vector(const _Tp & x); };
#if 1
     template <typename T1, typename T2> class pair { public: pair(T1 a, T2 b); };
     template <class Key, class T> class map { public: map(const pair<Key,T> & x); };
#endif
   }
#endif

// We need to define the base class where it is used in the std::vector<> template below.
// class AstAttribute {};
class AstAttribute { public: class AttributeEdgeInfo {}; };

// Note clear if we need this
// class dsl_attribute : public AstAttribute {};

#endif

#if 1
// These are independent of the target abstractions so they can be a
// part of this header file.

// Reference classe names that are specific to the DSL.
extern std::vector<std::string> dsl_type_names;

// Reference functions names that are specific to the DSL.
extern std::vector<std::string> dsl_function_names;

// Reference member functions (class and member function names) that are specific to the DSL.
extern std::vector< std::pair<std::string,std::string> > dsl_member_function_names;

// Forward declaration to support pointer to SgNode in the dsl_attribute class.
class SgNode;

// Example attributes that we need to have generated to support array DSL example.
class dsl_attribute : public AstAttribute
   {
     public:
          dsl_attribute();
          virtual ~dsl_attribute();

       // Factory copy mechanism.
       // virtual dsl_attribute* factory_copy() { return NULL; };
          virtual dsl_attribute* factory_copy();

          std::string toString();
          std::string additionalNodeOptions();

       // Most attributes will have children that define there embedding into the AST.
          std::vector<SgNode*> dslChildren;

          SgNode* currentNode;
          std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();

          std::string get_name();
          std::string get_color();
   };

// References to dsl attributes in a map inexed by the name of the dsl abstraction.
// extern std::map<std::string,dsl_attribute> dsl_attribute_map;
extern std::map<std::string,dsl_attribute*> dsl_attribute_map;
#endif

#if 0
// Examples of classes to be generated for to support the DSL compiler.
// NOTE: we might not need to generate this if it could be a template.
class array_dsl_attribute : public dsl_attribute
   {
     public:
       // Depending on the compile time semantics we want to leverage we may not need an object here.
          array value;

          array_dsl_attribute();
          virtual ~array_dsl_attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };
#endif
