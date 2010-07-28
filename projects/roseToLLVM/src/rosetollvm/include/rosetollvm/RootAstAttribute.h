#ifndef ROOT_AST_ATTRIBUTE
#define ROOT_AST_ATTRIBUTE

#include <rose.h>
#include <vector>
#include <rosetollvm/Control.h>

class RootAstAttribute : public AstAttribute { 
public: 

    ~RootAstAttribute() { 
        if (owner) {
            owner -> removeAttribute(code);
        }
    }

    SgNode *getOwner() { return owner; }
    const char *getCode() { return code; }

    void setOwner(SgNode *owner_, const char *code_) {
//        assert(owner);

        owner = owner_;
        code = code_;
    }

    void removeOwner() { owner = NULL; }

protected:

    SgNode *owner;
    const char *code;
};

#endif
