#include <rose.h>
#include "CodeProperties.h"

/** Finds calls to functions that open files and adds those calls to a list. */
class FileOpenFinder: public AstSimpleProcessing {
public:
    std::vector<SgFunctionCallExp*> found;

    virtual void visit(SgNode *node) /*override*/ {
        SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
        SgFunctionDeclaration *fdecl = fcall ? fcall->getAssociatedFunctionDeclaration() : NULL;
        std::string fname = fdecl ? fdecl->get_qualified_name().getString() : "";
        if (0==fname.compare("::fopen") || 0==fname.compare("::open") || 0==fname.compare("::creat") ||
            0==fname.compare("::openat")) {
            found.push_back(fcall);
            CodeProperties::message(std::cout, fcall, "file is opened");
        }
    }
};

int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    FileOpenFinder().traverse(project, preorder);
    return 0;
}
