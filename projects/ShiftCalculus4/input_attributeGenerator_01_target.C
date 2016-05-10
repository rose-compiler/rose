
// Include the API specification for DSL attributes so that we can more easily generate instantiations.
// This step could perhaps be automated, but it is not important at this point.
#include "dslAttribute.h"

// Example DSL API specification
class myString
   {
     private:
          char* internalString;

     public:
          myString(char* s);
     myString operator+(const myString & X) const;
   };

// Example of code that we want to generate (template instanitation directives):
extern template class DslAttribute<myString>;
