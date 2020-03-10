#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "BinaryDominance.h"

namespace Rose {
namespace BinaryAnalysis {

void
Dominance::clear_ast(SgNode *ast)
{
    struct T1: public AstSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block)
                block->set_immediate_dominator(NULL);
        }
    };
    T1().traverse(ast, preorder);
}

bool
Dominance::is_consistent(SgNode *ast, std::set<SgAsmBlock*> *bad_blocks/*=NULL*/)
{
    struct T1: public AstSimpleProcessing {
        bool failed;
        std::set<SgAsmBlock*> *bad_blocks;
        T1(std::set<SgAsmBlock*> *bad_blocks): failed(false), bad_blocks(bad_blocks) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            SgAsmFunction *func = block ? block->get_enclosing_function() : NULL;
            if (block && func) {
                if (block==func->get_entry_block()) {
                    if (block->get_immediate_dominator()) {
                        if (bad_blocks)
                            bad_blocks->insert(block);
                        failed = true;
                    }
                } else {
                    SgAsmBlock *idom = block->get_immediate_dominator();
                    if (!idom || idom->get_enclosing_function()!=func) {
                        if (bad_blocks)
                            bad_blocks->insert(block);
                        failed = true;
                    }
                }
            }
        }
    } t1(bad_blocks);
    t1.traverse(ast, preorder);
    return t1.failed;
}

} // namespace
} // namespace

#endif
