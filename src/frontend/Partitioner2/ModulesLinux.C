#include <sage3basic.h>
#include <Partitioner2/ModulesLinux.h>

#include <BinarySystemCall.h>
#include <DisassemblerX86.h>
#include <Partitioner2/Partitioner.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesLinux {

SystemCall
systemCallAnalyzer(const Partitioner &partitioner, const boost::filesystem::path &syscallHeader) {
    SystemCall analyzer;

    // We only support x86 Linux (32- or 64-bit)
    if (dynamic_cast<DisassemblerX86*>(partitioner.instructionProvider().disassembler())) {
        if (!syscallHeader.empty())
            analyzer.declare(syscallHeader);
        switch (partitioner.instructionProvider().instructionPointerRegister().nBits()) {
            case 32:
                if (syscallHeader.empty())
                    analyzer.declare("/usr/include/asm/unistd_32.h");
                analyzer.ident(AbstractLocation(RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 32)));
                break;
            case 64:
                if (syscallHeader.empty())
                    analyzer.declare("/usr/include/asm/unistd_64.h");
                analyzer.ident(AbstractLocation(RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 64)));
                break;
        }
    }
    
    return analyzer;
}

SyscallSuccessors::SyscallSuccessors(const Partitioner &partitioner, const boost::filesystem::path &syscallHeader)
    : analyzer_(systemCallAnalyzer(partitioner, syscallHeader)) {}

// class method
SyscallSuccessors::Ptr
SyscallSuccessors::instance(const Partitioner &partitioner, const boost::filesystem::path &syscallHeader) {
    return Ptr(new SyscallSuccessors(partitioner, syscallHeader));
}

bool
SyscallSuccessors::operator()(bool chain, const Args &args) {
    ASSERT_not_null(args.bblock);
    if (!chain)
        return false;
    if (args.bblock->nInstructions() == 0)
        return chain;

    SgAsmInstruction *syscallInsn = analyzer_.hasSystemCall(args.bblock);
    if (syscallInsn != args.bblock->instructions().back())
        return chain;

    try {
        SystemCall::Declaration decl;
        if (analyzer_.analyze(args.partitioner, args.bblock, syscallInsn, args.partitioner.smtSolver()).assignTo(decl)) {
            if (decl.name == "exit") {
                // This system call doesn't return
                args.bblock->successors(BasicBlock::Successors()); // defined, but empty
            } else {
                // This system call is like a function call in that it calls some undetermined location and then probably
                // returns to the fall through address. Therefore, create two edges: one for the function call and the other
                // for the call return.
                args.bblock->successors(BasicBlock::Successors()); // remove existing successors
                size_t wordsize = args.partitioner.instructionProvider().instructionPointerRegister().nBits();
                BaseSemantics::SValuePtr indeterminateVa = args.partitioner.newOperators()->undefined_(wordsize);
                args.bblock->insertSuccessor(indeterminateVa, E_FUNCTION_CALL);
                args.bblock->insertSuccessor(args.bblock->fallthroughVa(), wordsize, E_CALL_RETURN);
            }
        }
    } catch (...) {
        // Not an error if we can't figure out the system call name.
    }
    
    return chain;
}
            
void
nameSystemCalls(const Partitioner &partitioner, const boost::filesystem::path &syscallHeader) {
    SystemCall analyzer = systemCallAnalyzer(partitioner);
    BOOST_FOREACH (const BasicBlock::Ptr &bb, partitioner.basicBlocks()) {
        if (SgAsmInstruction *insn = analyzer.hasSystemCall(bb)) {
            SystemCall::Declaration decl;
            try {
                if (analyzer.analyze(partitioner, bb, insn, partitioner.smtSolver()).assignTo(decl))
                    insn->set_comment(decl.name);
            } catch (...) {
                // Not an error if we can't figure out the system call name.
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
