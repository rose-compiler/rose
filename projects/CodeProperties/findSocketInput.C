#include <rose.h>
#include "CodeProperties.h"

/** Finds function calls that open network sockets and adds those calls to a list. */
class SocketFinder: public AstSimpleProcessing {
public:
    std::vector<SgFunctionCallExp*> found;

    virtual void visit(SgNode *node) /*override*/ {
        SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
        SgFunctionDeclaration *fdecl = fcall ? fcall->getAssociatedFunctionDeclaration() : NULL;
        std::string fname = fdecl ? fdecl->get_qualified_name().getString() : "";
        if (0==fname.compare("::socket")) {
            found.push_back(fcall);
            CodeProperties::message(std::cout, fcall, "socket opened");
        }
    }
};

int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    SocketFinder().traverse(project, preorder);
    return 0;
}
