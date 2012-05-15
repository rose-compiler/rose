/* Tests whether an X86 instruction semantics policy can be subclassed. */

#include "rose.h"
#include "SymbolicSemantics.h"

using namespace BinaryAnalysis::InstructionSemantics;

class MyPolicy: public SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType> {
    typedef            SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType> super;
public:
    template<size_t nBits>
    SymbolicSemantics::ValueType<nBits> readRegister(const RegisterDescriptor &r) {
        std::cerr <<"Reading from register " <<get_register_dictionary()->lookup(r) <<"\n";
        return super::readRegister<nBits>(r);
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
    
