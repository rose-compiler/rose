#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <Partitioner2/ModulesLinux.h>

#include <BinarySystemCall.h>
#include <DisassemblerX86.h>
#include <Partitioner2/Partitioner.h>

using namespace Rose::Diagnostics;

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

bool
LibcStartMain::operator()(bool chain, const Args &args) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Look at this block only if it's a function call
    if (!chain || !args.bblock || args.bblock->nInstructions() == 0)
        return chain;
    if (!isSgAsmX86Instruction(args.bblock->instructions().back()))
        return chain;
    if (!args.partitioner.basicBlockIsFunctionCall(args.bblock))
        return chain;

    // It must call a function named "__libc_start_main@plt"
    bool foundCallToLibcStartMain = false;
    BOOST_FOREACH (const rose_addr_t &succVa, args.partitioner.basicBlockConcreteSuccessors(args.bblock)) {
        Function::Ptr func = args.partitioner.functionExists(succVa);
        if (func && func->name() == "__libc_start_main@plt") {
            foundCallToLibcStartMain = true;
            break;
        }
    }
    if (!foundCallToLibcStartMain)
        return chain;
    SAWYER_MESG(debug) <<"LibcStartMain analysis: found call to __libc_start_main\n";

    // One of the arguments to the function call is the address of "main". We need instruction semantics to get its value.
    // Map-based memory seems to work best for this because it's faster, simpler to understand, and avoids inconsistencies when
    // the basic block has aliasing near the top of the stack.  Since __libc_start_main is called by _start, which is also
    // responsible for aligning the stack pointer, the stack pointer ends up often being an somewhat complicated symbolic
    // expression, which in turn causes aliasing-aware memory states to return unknown values when reading the stack (using
    // similar complicated expression) due to possible aliasing.
    BaseSemantics::DispatcherPtr dispatcher;
    BaseSemantics::RiscOperatorsPtr ops;
    BaseSemantics::StatePtr state;
    try {
        ops = args.partitioner.newOperators(MAP_BASED_MEMORY);
        if (ops) {
            dispatcher = args.partitioner.newDispatcher(ops);
            if (dispatcher) {
                BOOST_FOREACH (SgAsmInstruction *insn, args.bblock->instructions())
                    dispatcher->processInstruction(insn);
                state = dispatcher->currentState();
            }
        }
    } catch (...) {
    }
    if (!state) {
        SAWYER_MESG(debug) <<"LibcStartMain analysis: failed to obtain basic block semantic state\n";
        return chain;
    }

    // Location and size of argument varies by architecture
    Semantics::SValuePtr mainVa;
    if (isSgAsmX86Instruction(args.bblock->instructions().back())) {
        if (dispatcher->addressWidth() == 64) {
            const RegisterDescriptor REG_RCX = dispatcher->findRegister("rcx", 64, true /*allowMissing*/);
            ASSERT_require(!REG_RCX.isEmpty());

            BaseSemantics::SValuePtr rcx = state->peekRegister(REG_RCX, dispatcher->undefined_(64), ops.get());
            if (rcx->is_number())
                mainVa = Semantics::SValue::promote(rcx);

        } else if (dispatcher->addressWidth() == 32) {
            dispatcher->get_operators()->currentState(state);
            const RegisterDescriptor REG_ESP = args.partitioner.instructionProvider().stackPointerRegister();
            ASSERT_require(!REG_ESP.isEmpty());
            BaseSemantics::SValuePtr esp = dispatcher->get_operators()->peekRegister(REG_ESP, dispatcher->undefined_(32));
            BaseSemantics::SValuePtr arg0addr = dispatcher->get_operators()->add(esp, dispatcher->number_(32, 4));
            BaseSemantics::SValuePtr arg0 = dispatcher->get_operators()->peekMemory(RegisterDescriptor(),
                                                                                        arg0addr, dispatcher->undefined_(32));
            SAWYER_MESG(debug) <<"LibcStartMain analysis: x86-32 arg0 addr  = " <<*arg0addr <<"\n"
                               <<"LibcStartMain analysis: x86-32 arg0 value = " <<*arg0 <<"\n";
            if (arg0->is_number())
                mainVa = Semantics::SValue::promote(arg0);

        } else {
            // architecture not supported yet
        }
    }

    if (mainVa) {
        ASSERT_require(args.bblock->successors().isCached());
        SAWYER_MESG(debug) <<"LibcStartMain analysis: address of \"main\" is " <<*mainVa <<"\n";
        BasicBlock::Successors succs = args.bblock->successors().get();
        succs.push_back(BasicBlock::Successor(mainVa, E_FUNCTION_CALL));
        args.bblock->successors() = succs;

        if (mainVa->is_number() && mainVa->get_width() <= 64)
            mainVa_ = mainVa->get_number();
    }

    return true;
}

void
LibcStartMain::nameMainFunction(const Partitioner &partitioner) const {
    if (mainVa_) {
        if (Function::Ptr main = partitioner.functionExists(*mainVa_)) {
            if (main->name().empty())
                main->name("main");
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
