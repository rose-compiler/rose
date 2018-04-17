#ifndef CONDITIONAL_AST_ATTRIBUTE
#define CONDITIONAL_AST_ATTRIBUTE

#include <rosetollvm/IfAstAttribute.h>

class ConditionalAstAttribute : public IfAstAttribute { 
public: 
    ConditionalAstAttribute(std::string t, std::string f, std::string e) : IfAstAttribute(t, f, e)
    {} 

    ConditionalAstAttribute(ConditionalAstAttribute *a) : IfAstAttribute(a)
    {} 
};

#endif
