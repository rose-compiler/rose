#ifndef INT_AST_ATTRIBUTE
#define INT_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class IntAstAttribute : public RootAstAttribute { 
public: 
    IntAstAttribute(int s) : value(s) {} 

    int getValue() { return value; }

private:

    int value;
};

#endif

