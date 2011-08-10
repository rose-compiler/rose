#include "sage3basic.h"
#include "BinaryControlFlow.h"

#include <boost/graph/depth_first_search.hpp>

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::clear_ast(SgNode *root)
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
