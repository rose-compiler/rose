#ifndef DECLARATIONS_AST_ATTRIBUTE
#define DECLARATIONS_AST_ATTRIBUTE

#include <string>
#include <vector>
#include <rosetollvm/StringSet.h>
#include <rose.h>
#include <rosetollvm/RootAstAttribute.h>

class DeclarationsAstAttribute : public RootAstAttribute { 
public: 

    DeclarationsAstAttribute(SgClassType *n = NULL) : class_type(n) {} 

    int numSgInitializedNames() { return decls.size(); }
    SgInitializedName *getSgInitializedName(int i) { return decls[i]; }

    void addSgInitializedName(SgInitializedName *decl) { decls.push_back(decl); }

    bool containsName(std::string name) { return name_set.contains(name.c_str()); }
    void insertName(std::string name)   { name_set.insert(name.c_str()); }
    void insertPadding() {
        std::stringstream out;
        out << "%" << name_set.size(); // a unique name
        name_set.insert(out.str().c_str());
    }
    void insertPadding(int pad_size) {
        for (int i = 0; i < pad_size; i++) {
            insertPadding();
        }
    }
    int nameIndex(std::string name) { return name_set.getIndex(name.c_str()); }

    SgClassType *getClassType() { return class_type; }

protected:

    std::vector<SgInitializedName *> decls;
    StringSet name_set;

    SgClassType *class_type;
};

#endif


