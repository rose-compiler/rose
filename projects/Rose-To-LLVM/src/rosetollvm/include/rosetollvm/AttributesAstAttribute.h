#ifndef ATTRIBUTE_AST_ATTRIBUTE
#define ATTRIBUTE_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class AttributesAstAttribute : public RootAstAttribute { 
public: 
    AttributesAstAttribute(LLVMAstAttributes *a) : value(a) {} 

    LLVMAstAttributes *getValue() { return value; }

private:

    LLVMAstAttributes* value;
};

#endif
