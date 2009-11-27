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
    ROSE_ASSERT(f->get_statementList().size()>0);
    SgAsmBlock *first_bb = isSgAsmBlock(f->get_statementList().front());
    sprintf(buf, "0x%"PRIx64, first_bb->get_id());
    return buf;
}

class AnalyzeFunctions : public SgSimpleProcessing {
  public:
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

/* Analyze only functions that are defined in an interpretation that includes a Windows PE file header. */
class AnalyzePeFunctions: public SgSimpleProcessing {
  public:
    void visit(SgNode *node) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(node);
        if (interp) {
            const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
            bool contains_pe_header = false;
            for (size_t i=0; i<headers.size() && !contains_pe_header; ++i)
                contains_pe_header = isSgAsmPEFileHeader(headers[i])!=NULL;
            if (contains_pe_header) {
                std::cout <<"Found a PE file header\n";
                AnalyzeFunctions().traverse(node, postorder);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    SgProject *project = frontend(argc, argv);
#if 1
    /* Analyze only functions under a Windows PE File Header */
    AnalyzePeFunctions().traverse(project, postorder);
#else
    /* Analyze all functions */
    AnalyzeFunctions().traverse(project, postorder);
#endif
}
