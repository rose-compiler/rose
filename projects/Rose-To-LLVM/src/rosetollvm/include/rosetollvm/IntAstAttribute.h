#ifndef INT_AST_ATTRIBUTE
#define INT_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class IntAstAttribute : public RootAstAttribute { 
public: 
    IntAstAttribute(long long s) : value(s) {} 

    long long getValue() { return value; }
    void resetValue(long long value_) { value = value_; }

private:

    long long value;
};

#endif

