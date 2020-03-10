#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "BinaryControlFlow.h"

#include <boost/graph/depth_first_search.hpp>

namespace Rose {
namespace BinaryAnalysis {

/* See header file for documentation. */
void
ControlFlow::clear_ast(SgNode *root)
{
    struct T1: public AstSimpleProcessing {
        ControlFlow *analyzer;
        T1(ControlFlow *analyzer): analyzer(analyzer) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block && !analyzer->is_vertex_filtered(block)) {
                block->get_successors().clear();
                block->set_successors_complete(false);
            }
        }
    };
    T1(this).traverse(root, preorder);
}

} // namespace
} // namespace

#endif
