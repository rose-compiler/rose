// This test code demonstrates where the non-defining declaration
// can follow the defining declaration for a number or C++ constructs.
// This case was previously trapped as an error in the AST fixup phase.
 
// template declaration (definition)
template<typename T>
class templated_class_declaration
   {
     public:
         T t;
   };

// forward declaration following the defining declaration!
template<typename T>
class templated_class_declaration;


enum numbers {};

// This demonstrates that each call to build a non-defining 
// enum declaration must gnerate a unique declaration.
enum numbers;
enum numbers;
