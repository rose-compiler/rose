
// Simple hand-written example of the DSL attributes class that we want to generate automatically.

// Reference classes that are specific to the DSL.
extern char* dsl_types[1];

// Reference functions that are specific to the DSL.
extern char* dsl_functions[1];

// Reference member functions (class and member function) that are specific to the DSL.
extern char* dsl_member_functions[1][2];

// Example attributes that we need to have generated to support array DSL example.

class array_dsl_attribute : public AstAttribute
   {
     public:
       // array value;
   };

