
// Include the API specification for DSL attributes so that we can more easily generate instantiations.
// This step could perhaps be automated, but it is not important at this point.
#include "dslAttribute.h"

// Example DSL API specification
#pragma DSL keyword
class myString
   {
     private:
          char* internalString;

     public:
          myString(char* s);
       // myString operator+(const myString & X) const;
       // myString foobar(const myString & X) const;
          void foobar();
   };

// Code that we want to be generated:

// Template specialization forward declaration ???:
// template<> class DslAttribute<myString>;

// Template instantiation directive:
extern template class DslClassAttribute<myString>;

// Template instantiation forward declaration ???:
// class DslAttribute<myString>;


// Alternative template arguments
// void (Bar::*BarSetterFunction)(const BazReturnType &)
// extern template class DslAttribute<myString::*(const myString &) const>;
// extern template class DslMemberFunctionAttribute<myString,void (myString::*templateFunctionArgument)(const myString &) const>;
// extern template class DslMemberFunctionAttribute<myString,myString (myString::foobar)(const myString &)>;
// extern template class DslMemberFunctionAttribute<myString,void (&myString::foobar)()>;
extern template class DslMemberFunctionAttribute<myString,void,&myString::foobar>;

// Template instantiation directive for non-member functions.
void global_foobar();
extern template class DslFunctionAttribute<void,&global_foobar>;
