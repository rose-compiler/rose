#include <rose.h>
#include <sys/mman.h>

#include "CodeProperties.h"

/** Finds function calls that open network sockets and adds those calls to a list. */
class SharedMemoryFinder: public AstSimpleProcessing {
public:
    std::vector<SgFunctionCallExp*> found;

    virtual void visit(SgNode *node) /*override*/ {
        SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
        SgFunctionDeclaration *fdecl = fcall ? fcall->getAssociatedFunctionDeclaration() : NULL;
        std::string fname = fdecl ? fdecl->get_qualified_name().getString() : "";
        if (0==fname.compare("::mmap")) {
            // Look for shared memory via mmap(). Third argument should include the MAP_SHARED bit. We don't do anything
            // fancty--the MAP_SHARED bit must be literal (which it normally is).
            const SgExpressionPtrList &args = fcall->get_args()->get_expressions();
            if (args.size()==6 && isSgValueExp(args[3])) {
                uint64_t flags = 0;
                if (isSgIntVal(args[3])) {
                    flags = isSgIntVal(args[3])->get_value();
                } else if (isSgLongIntVal(args[3])) {
                    flags = isSgLongIntVal(args[3])->get_value();
                } else if (isSgLongLongIntVal(args[3])) {
                    flags = isSgLongLongIntVal(args[3])->get_value();
                } else if (isSgShortVal(args[3])) {
                    flags = isSgShortVal(args[3])->get_value();
                } else if (isSgUnsignedIntVal(args[3])) {
                    flags = isSgUnsignedIntVal(args[3])->get_value();
                } else if (isSgUnsignedLongLongIntVal(args[3])) {
                    flags = isSgUnsignedLongLongIntVal(args[3])->get_value();
                } else if (isSgUnsignedLongVal(args[3])) {
                    flags = isSgUnsignedLongVal(args[3])->get_value();
                } else if (isSgUnsignedShortVal(args[3])) {
                    flags = isSgUnsignedShortVal(args[3])->get_value();
                }
                if (flags & MAP_SHARED) { // assuming MAP_SHARED is the same for ROSE and the specimen
                    found.push_back(fcall);
                    CodeProperties::message(std::cout, fcall, "shared memory created");
                }
            }
        }
    }
};

int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    SharedMemoryFinder().traverse(project, preorder);
    return 0;
}
