#ifndef MANAGER_AST_ATTRIBUTE
#define MANAGER_AST_ATTRIBUTE

#include <assert.h>
#include <rose.h>
#include <vector>
#include <rosetollvm/RootAstAttribute.h>

class ManagerAstAttribute : public RootAstAttribute { 
public: 
  ManagerAstAttribute(SgNode *owner, const char *code) {
        setOwner(owner, code);
        starting_number_of_attributes = owner -> numberOfAttributes();
    }

    ManagerAstAttribute(ManagerAstAttribute *manager) {
        //  ...
    }

    ~ManagerAstAttribute() {
        for (int i = 0; i < attributes.size(); i++) {
            RootAstAttribute *attribute = attributes[i];
            delete attribute;
        }
    }

    void addAttribute(RootAstAttribute *attribute) {
        attributes.push_back(attribute);
    }

    int startingNumberOfAttributes() { return starting_number_of_attributes; }

private:
    std::vector<RootAstAttribute *> attributes;
    int starting_number_of_attributes;
};

#endif
