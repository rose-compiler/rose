/* Tests whether an X86 instruction semantics policy can be subclassed. */

#include "rose.h"
#include "SymbolicSemantics.h"

class MyPolicy: public SymbolicSemantics::Policy {
public:
    SymbolicSemantics::ValueType<32> readGPR(X86GeneralPurposeRegister r) const {
        std::cerr <<"Reading from register.\n";
        return SymbolicSemantics::Policy::readGPR(r);
    }
};

class Analyzer: public AstSimpleProcessing {
    MyPolicy policy;
    X86InstructionSemantics<MyPolicy, SymbolicSemantics::ValueType> semantics;
public:
    Analyzer(): semantics(policy) {}

    void visit(SgNode *node) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(node);
        if (insn)
            semantics.processInstruction(insn);
    }
};

int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    Analyzer analyzer;
    analyzer.traverse(project, postorder);
    return 0;
}
    
