#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"

#include "SymbolicSemantics2.h"
#include "DispatcherX86.h"

using namespace Rose::BinaryAnalysis;

struct Analysis: public AstSimpleProcessing {
    void visit(SgNode *node) {
        SgAsmBlock *block = isSgAsmBlock(node);
        if (block && block->has_instructions()) {
            using namespace Rose::BinaryAnalysis::InstructionSemantics2;
            const RegisterDictionary *regdict = RegisterDictionary::dictionary_i386();
            SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(regdict);
            ops->computingDefiners(SymbolicSemantics::TRACK_ALL_DEFINERS); // only used so we can test that it works
            BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(ops, 32);
            const SgAsmStatementPtrList &stmts = block->get_statementList();
            for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(*si);
                if (insn) {
                    std::cout <<unparseInstructionWithAddress(insn) <<"\n";
                    dispatcher->processInstruction(insn);
                    std::cout <<*ops <<"\n";
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

#endif
