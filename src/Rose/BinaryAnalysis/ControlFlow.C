#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>                                 // needed by the ControlFlow.h header
#include <Rose/BinaryAnalysis/ControlFlow.h>

#include <Rose/AST/Traversal.h>

namespace Rose {
namespace BinaryAnalysis {

void
ControlFlow::clear_ast(SgNode *root) {
    AST::Traversal::forwardPre<SgAsmBlock>(root, [this](SgAsmBlock *block) {
        if (block && !is_vertex_filtered(block)) {
            block->get_successors().clear();
            block->set_successorsComplete(false);
        }
    });
}

} // namespace
} // namespace

#endif
