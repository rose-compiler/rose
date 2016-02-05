
// Simple hand-written example of the DSL attributes class that we want to generate automatically.

// Reference classe names that are specific to the DSL.
extern std::vector<std::string> dsl_type_names;

// Reference functions names that are specific to the DSL.
extern std::vector<std::string> dsl_function_names;

// Reference member functions (class and member function names) that are specific to the DSL.
extern std::vector< std::pair<std::string,std::string> > dsl_member_function_names;

// Example attributes that we need to have generated to support array DSL example.
class array_dsl_attribute : public AstAttribute
   {
     public:
       // array value;
   };

