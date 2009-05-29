/* For each function (SgAsmFunctionDeclaration) process each instruction (SgAsmInstruction) through the instruction semantics
 * layer using the FindConstantsPolicy. Output consists of each instruction followed by the registers and memory locations
 * with constant or pseudo-constant values. */

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include "findConstants.h"
#include <inttypes.h>

/* Returns the function name if known, or the address as a string otherwise. */
static std::string
name_or_addr(const SgAsmFunctionDeclaration *f)
{
    if (f->get_name()!="")
        return f->get_name();

    char buf[128];
    SgAsmBlock *first_bb = isSgAsmBlock(f->get_statementList().front());
    sprintf(buf, "0x%"PRIx64, first_bb->get_id());
    return buf;
}

class AnalyzeFunctions : public SgSimpleProcessing {
  public:
    AnalyzeFunctions(SgProject *project) {
        traverse(project, postorder);
    }
    void visit(SgNode *node) {
        SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
        if (func) {
            std::cout <<"==============================================\n"
                      <<"Constant propagation in function \"" <<name_or_addr(func) <<"\"\n"
                      <<"==============================================\n";
            FindConstantsPolicy policy;
            X86InstructionSemantics<FindConstantsPolicy, XVariablePtr> t(policy);
            std::vector<SgNode*> instructions = NodeQuery::querySubTree(func, V_SgAsmx86Instruction);
            for (size_t i=0; i<instructions.size(); i++) {
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(instructions[i]);
                ROSE_ASSERT(insn);
                t.processInstruction(insn);
                RegisterSet rset = policy.currentRset;
                std::cout <<unparseInstructionWithAddress(insn) <<"\n"
                          <<rset;
            }
        }
    }
};

int main(int argc, char *argv[]) {
    AnalyzeFunctions(frontend(argc, argv));
}
