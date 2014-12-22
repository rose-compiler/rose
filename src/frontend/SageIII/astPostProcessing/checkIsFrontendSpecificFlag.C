#include "sage3basic.h"
#include "checkIsFrontendSpecificFlag.h"
#include "stringify.h"

using namespace rose;

// documented in header file
size_t
checkIsFrontendSpecificFlag(SgNode *ast)
{
    struct T1: public AstPrePostProcessing {
        SgNode *fes_ast; // top node of frontend-specific AST
        size_t nviolations;
        T1(): fes_ast(NULL), nviolations(0) {}

        // Start marking nodes as frontend-specific once we enter an AST that's frontend-specific.
        void preOrderVisit(SgNode *node) {
            SgLocatedNode *located = isSgLocatedNode(node);
            if (located) {
                bool in_fes_ast = fes_ast!=NULL ||
                                  is_frontend_specific(located->get_file_info()) ||
                                  is_frontend_specific(located->generateMatchingFileInfo()) ||
                                  is_frontend_specific(located->get_startOfConstruct()) ||
                                  is_frontend_specific(located->get_endOfConstruct());
                if (in_fes_ast) {
                    if (!fes_ast)
                        fes_ast = node;
                    fix(located, located->get_file_info());
                    fix(located, located->generateMatchingFileInfo());
                    fix(located, located->get_startOfConstruct());
                    fix(located, located->get_endOfConstruct());
                }
            }
        }

        // Figure out when we exit the frontend-specific AST
        void postOrderVisit(SgNode *node) {
            if (node==fes_ast)
                fes_ast = NULL;
        }

        // Criteria for deciding whether we're entering the top of an AST that's frontend-specific.
        bool is_frontend_specific(Sg_File_Info *finfo) {
            static const char *header_name = "/rose_edg_required_macros_and_functions.h";
            return finfo && std::string::npos!=finfo->get_filenameString().rfind(header_name);
        }

        // Mark node as frontend-specific and emit a warning if it wasn't already so marked.
        void fix(SgNode *node, Sg_File_Info *finfo) {
            if (finfo && !finfo->isFrontendSpecific()) {
#if 0
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                std::cerr <<finfo->get_filenameString() <<":" <<finfo->get_line() <<"." <<finfo->get_col() <<": "
                          <<"node should be marked as frontend-specific: "
                          <<"(" <<stringifyVariantT(node->variantT(), "V_") <<"*)" <<node <<"\n";
#endif
#endif
                finfo->setFrontendSpecific();
                ++nviolations;
            }
        }
    } t1;

    t1.traverse(ast);
    return t1.nviolations;
}
