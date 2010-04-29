/* For each function (SgAsmFunctionDeclaration) process each instruction (SgAsmInstruction) through the instruction semantics
 * layer using the FindConstantsPolicy. Output consists of each instruction followed by the registers and memory locations
 * with constant or pseudo-constant values. */

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include "findConstants.h"
#include <set>
#include <inttypes.h>

#ifndef BASE_POLICY
#define BASE_POLICY FindConstantsPolicy
#endif

struct TestPolicy: public BASE_POLICY {
    void startInstruction(SgAsmInstruction *insn) {
        addr = insn->get_address();
        newIp = number<32>(addr);
        if (rsets.find(addr)==rsets.end()) {
            rsets[addr].setToBottom();
            std::cout <<"Initial state:\n" <<rsets[addr];
        }
        currentRset = rsets[addr];
        currentInstruction = isSgAsmx86Instruction(insn);
    }
};

/* Analyze a single interpretation a block at a time */
static void
analyze_interp(SgAsmInterpretation *interp)
{
    typedef X86InstructionSemantics<TestPolicy, XVariablePtr> Semantics;

    /* Get the set of all instructions */
    struct AllInstructions: public SgSimpleProcessing, public std::map<rose_addr_t, SgAsmx86Instruction*> {
        void visit(SgNode *node) {
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(node);
            if (insn) insert(std::make_pair(insn->get_address(), insn));
        }
    } insns;
    insns.traverse(interp, postorder);

    while (!insns.empty()) {
        std::cout <<"=====================================================================================\n"
                  <<"=== Starting a new basic block                                                    ===\n"
                  <<"=====================================================================================\n";
        AllInstructions::iterator si = insns.begin();
        SgAsmx86Instruction *insn = si->second;
        insns.erase(si);

        TestPolicy policy;
        Semantics semantics(policy);


        /* Perform semantic analysis for each instruction in this block. The block ends when we no longer know the value of
         * the instruction pointer or the instruction pointer refers to an instruction that doesn't exist or which has already
         * been processed. */
        while (1) {
            /* Analyze current instruction */
            try {
                semantics.processInstruction(insn);
                RegisterSet rset = policy.currentRset;
                std::cout <<unparseInstructionWithAddress(insn) <<"\n"
                          <<rset
                          <<"    ip = " <<policy.newIp <<"\n";
            } catch (const Semantics::Exception &e) {
                std::cout <<e.mesg <<": " <<unparseInstructionWithAddress(e.insn) <<"\n";
                break;
            }

            /* Never follow CALL instructions */
            if (insn->get_kind()==x86_call || insn->get_kind()==x86_farcall)
                break;

            /* Get next instruction of this block */
            if (policy.newIp->get().name) break;
            rose_addr_t next_addr = policy.newIp->get().offset;
            si = insns.find(next_addr);
            if (si==insns.end()) break;
            insn = si->second;
            insns.erase(si);
        }
    }
}

/* Analyze only interpretations that point only to 32-bit x86 instructions. */
class AnalyzeX86Functions: public SgSimpleProcessing {
public:
    size_t ninterps;
    AnalyzeX86Functions(): ninterps(0) {}
    void visit(SgNode* node) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(node);
        if (interp) {
            const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
            bool only_x86 = true;
            for (size_t i=0; i<headers.size() && only_x86; ++i)
                only_x86 = 4==headers[i]->get_word_size();
            if (only_x86) {
                ++ninterps;
                analyze_interp(interp);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    SgProject *project = frontend(argc, argv);
    AnalyzeX86Functions analysis;
    analysis.traverse(project, postorder);
    if (0==analysis.ninterps) {
        std::cout <<"file(s) didn't have any 32-bit x86 headers.\n";
    } else {
        std::cout <<"analyzed headers: " <<analysis.ninterps<< "\n";
    }
}
