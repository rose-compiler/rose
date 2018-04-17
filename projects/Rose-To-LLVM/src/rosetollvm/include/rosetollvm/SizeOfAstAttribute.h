#ifndef SIZEOF_AST_ATTRIBUTE
#define SIZEOF_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class SizeOfAstAttribute : public RootAstAttribute { 
public: 
    SizeOfAstAttribute(SgSizeOfOp *n) : value(n) {} 

    SgSizeOfOp *getValue() { return value; }

private:

    SgSizeOfOp *value;
};

#endif
