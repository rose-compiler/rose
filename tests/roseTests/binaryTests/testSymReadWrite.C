#include "rose.h"

#include "SymbolicSemantics.h"
#include "YicesSolver.h"

struct Analysis: public AstSimpleProcessing {
    void visit(SgNode *node) {
        SgAsmBlock *block = isSgAsmBlock(node);
        if (block && block->has_instructions()) {
            
            YicesSolver smt_solver;
            smt_solver.set_linkage(YicesSolver::LM_EXECUTABLE); // more debugging capabilities

            using namespace BinaryAnalysis::InstructionSemantics;
            typedef SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType> Policy;
            typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;

            Policy policy(&smt_solver);
            Semantics semantics(policy);

            const SgAsmStatementPtrList &stmts = block->get_statementList();
            for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(*si);
                if (insn) {
                    std::cout <<unparseInstructionWithAddress(insn) <<"\n";
                    semantics.processInstruction(insn);
                    std::cout <<policy;
                }
            }
        }
    }
};

int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    Analysis().traverse(project, preorder);
}

