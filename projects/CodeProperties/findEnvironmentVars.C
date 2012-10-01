#include <rose.h>
#include "CodeProperties.h"

/** Finds calls to functions that return environment variables and adds those calls to a list. */
class EnvironmentVariableFinder: public AstSimpleProcessing {
public:
    std::vector<SgFunctionCallExp*> found;              /**< List of function calls that return environment variables. */

    virtual void visit(SgNode *node) /*override*/ {
        SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
        SgFunctionDeclaration *fdecl = fcall ? fcall->getAssociatedFunctionDeclaration() : NULL;
        std::string fname = fdecl ? fdecl->get_qualified_name().getString() : "";
        if (SageInterface::is_Java_language()) {
            if (0==fname.compare("System.getenv")) {
                found.push_back(fcall);
                CodeProperties::message(std::cout, fcall, "environment variable is read");
            }
        } else if (0==fname.compare("::getenv")) {
            found.push_back(fcall);
            CodeProperties::message(std::cout, fcall, "environment variable is read");
        }
    }
};

int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    EnvironmentVariableFinder().traverse(project, preorder);
    return 0;
}
