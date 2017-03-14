#ifndef SG_TYPE_AST_ATTRIBUTE
#define SG_TYPE_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class SgTypeAstAttribute : public RootAstAttribute { 
public: 
    SgTypeAstAttribute(SgType *type_) : type(type_) {} 

    SgType *getType() { return type; }

    void resetType(SgType *type_) { type = type_; }

private:

    SgType *type;
};

#endif
