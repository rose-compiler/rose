#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"

using namespace rose;

struct AnalyzeBlocks: AstSimpleProcessing {
    void visit(SgNode *node) {
        using namespace StringUtility;
        if (SgAsmBlock *bb = isSgAsmBlock(node)) {
            if (bb->is_basic_block()) {
                std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(bb);
                for (size_t i=0; i<insns.size(); ++i)
                    std::cout <<"| " <<unparseInstruction(insns[i]) <<"\n";

                rose_addr_t target_va=911, return_va=911;
                bool is_call = bb->is_function_call(target_va, return_va);
                std::cout <<addrToString(bb->get_address(), 64) <<": " <<(is_call ? " is_call" : "not_call")
                          <<" target=" <<addrToString(target_va, 64) <<" return=" <<addrToString(return_va, 64) <<"\n\n";
            }
        }
    }
};


int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    AnalyzeBlocks().traverse(project, preorder);
    backend(project);
    return 0;
}

#endif
