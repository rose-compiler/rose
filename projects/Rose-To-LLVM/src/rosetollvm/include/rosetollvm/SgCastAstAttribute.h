#ifndef SG_CAST_AST_ATTRIBUTE
#define SG_CAST_AST_ATTRIBUTE

#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class SgCastAstAttribute : public RootAstAttribute { 
public: 
    SgCastAstAttribute(SgType *type_, std::string name_) : type(type_),
                                                           name(name_)
    {} 

    SgType *getType() { return type; }
    void resetType(SgType *type_) { type = type_; }


    std::string getName() { return name; }
    void resetName(std::string name_) { name = name_; }

private:

    SgType *type;
    std::string name;
};

#endif
