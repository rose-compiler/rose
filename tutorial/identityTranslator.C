// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include <boost/foreach.hpp>

static SgInitializedName *
findInitializedName(SgNode *ast, const std::string &name)
{
    std::vector<SgInitializedName*> names = SageInterface::querySubTree<SgInitializedName>(ast);
    BOOST_FOREACH (SgInitializedName *iname, names) {
        if (0==iname->get_name().getString().compare(name))
            return iname;
    }
    return NULL;
}


// tests for Unparse_Java::unparseInitializedName
    

int main(int argc, char * argv[]) {
    SgProject* project = frontend(argc,argv);
    AstTests::runAllTests(project);

    // AST returned by frontend() is ok.
    SgInitializedName *a = findInitializedName(project, "a");
    ROSE_ASSERT(a->attributeExists("type") || !"a must have a type attribute");
    (void) a->getAttribute("type");

    // Copy part of the AST
    SgTreeCopy deep;
    SgInitializedName *b = isSgInitializedName(a->copy(deep));
    ROSE_ASSERT(b || !"copy failed");

    // Check the copy
    ROSE_ASSERT(b->attributeExists("type") || !"copy has no type attribute");
    (void) b->getAttribute("type");                     // fails assertion
}
