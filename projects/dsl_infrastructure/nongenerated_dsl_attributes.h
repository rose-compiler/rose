// Simple hand-written example of the DSL attributes class that we want to generate automatically.
#if 0
// Reference classe names that are specific to the DSL.
extern std::vector<std::string> dsl_type_names;

// Reference functions names that are specific to the DSL.
extern std::vector<std::string> dsl_function_names;

// Reference member functions (class and member function names) that are specific to the DSL.
extern std::vector< std::pair<std::string,std::string> > dsl_member_function_names;

// Example attributes that we need to have generated to support array DSL example.
class dsl_attribute : public AstAttribute
   {
     public:
          dsl_attribute();
          virtual ~dsl_attribute();

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
extern std::map<std::string,dsl_attribute> dsl_attribute_map;
#endif

#if 1
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

class array_refexp_dsl_attribute : public dsl_attribute
   {
     public:
       // array_refexp_dsl_attribute();
       // virtual ~array_refexp_dsl_attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class value_refexp_dsl_attribute : public dsl_attribute
   {
     public:
       // array_refexp_dsl_attribute();
       // virtual ~array_refexp_dsl_attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class array_assignment_operator_dsl_attribute : public dsl_attribute
   {
     public:
          std::string toString();
          std::string additionalNodeOptions();
   };

class scalar_assignment_operator_dsl_attribute : public dsl_attribute
   {
     public:
          std::string toString();
          std::string additionalNodeOptions();
   };

class plus_operator_dsl_attribute : public dsl_attribute
   {
     public:
          std::string toString();
          std::string additionalNodeOptions();
   };


