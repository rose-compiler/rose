#include "rose.h"
#include "SymbolicSemantics.h"

struct Analysis: public AstSimpleProcessing {
    void visit(SgNode *node) {
        SgAsmBlock *block = isSgAsmBlock(node);
        if (block && block->has_instructions()) {
            using namespace BinaryAnalysis::InstructionSemantics;
            typedef SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType> Policy;
            typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;

            Policy policy(NULL/*no SMT solver*/);
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

