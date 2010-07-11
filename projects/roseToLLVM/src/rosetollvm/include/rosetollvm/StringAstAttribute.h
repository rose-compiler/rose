#ifndef STRING_AST_ATTRIBUTE
#define STRING_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class StringAstAttribute : public RootAstAttribute { 
public: 
    StringAstAttribute(std::string s) : value(s) {} 

    const std::string getValue() { return value; }

    void resetValue(std::string value_) { value = value_; }

private:

    std::string value;
};

#endif
