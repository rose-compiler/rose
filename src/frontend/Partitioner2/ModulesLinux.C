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

    // Some of the arguments are going to be function pointers. Give them names if we can.
    std::vector<BaseSemantics::SValuePtr> functionPtrs;

    // Location and size of argument varies by architecture
    const RegisterDictionary *regs = args.partitioner.instructionProvider().registerDictionary();
    if (isSgAsmX86Instruction(args.bblock->instructions().back())) {
        if (dispatcher->addressWidth() == 64) {
            // Amd64 integer arguments are passed in registers: rdi, rsi, rdx, rcx, r8, and r9
            const RegisterDescriptor FIRST_ARG = regs->findOrThrow("rdi");
            const RegisterDescriptor FOURTH_ARG = regs->findOrThrow("rcx");
            const RegisterDescriptor FIFTH_ARG = regs->findOrThrow("r8");
            BaseSemantics::SValuePtr firstArg = state->peekRegister(FIRST_ARG, dispatcher->undefined_(64), ops.get());
            BaseSemantics::SValuePtr fourthArg = state->peekRegister(FOURTH_ARG, dispatcher->undefined_(64), ops.get());
            BaseSemantics::SValuePtr fifthArg = state->peekRegister(FIFTH_ARG, dispatcher->undefined_(64), ops.get());

            if (firstArg->is_number() && fourthArg->is_number() && fifthArg->is_number() &&
                args.partitioner.memoryMap()->at(firstArg->get_number()).require(MemoryMap::EXECUTABLE).exists() &&
                args.partitioner.memoryMap()->at(fourthArg->get_number()).require(MemoryMap::EXECUTABLE).exists() &&
                args.partitioner.memoryMap()->at(fifthArg->get_number()).require(MemoryMap::EXECUTABLE).exists()) {
                // Sometimes the address of main is passed as the first argument (rdi) with __libc_csu_init and __libc_csu_fini
                // passed as the fourth (rcx) and fifth (r8) arguments. By this point in the disassembly process, would not
                // have discovered PLT function yet. So if the first, fourth, and fifth arguments seem to point at executable
                // memory then assume they are "main", "__libc_csu_fini@plt", and "libc_csu_init@plt". Don't bother naming the
                // two PLT functions though since we'll get their names later by processing the PLT.
                SAWYER_MESG(debug) <<"LibcStartMain analysis: amd64 with main as the fisrt argument\n";
                mainVa_ = firstArg->get_number();
                functionPtrs.push_back(firstArg);
                functionPtrs.push_back(fourthArg);
                functionPtrs.push_back(fifthArg);

            } else if (fourthArg->is_number() &&
                       args.partitioner.memoryMap()->at(fourthArg->get_number()).require(MemoryMap::EXECUTABLE).exists()) {
                // Sometimes then address of main is passed as the fourth argument (in rcx).
                SAWYER_MESG(debug) <<"LibcStartMain analysis: amd64 with main as the fourth argument\n";
                mainVa_ = fourthArg->get_number();
                functionPtrs.push_back(fourthArg);
            }

        } else if (dispatcher->addressWidth() == 32) {
            // x86 integer arguments are passed on the stack
            dispatcher->get_operators()->currentState(state);
            const RegisterDescriptor REG_ESP = args.partitioner.instructionProvider().stackPointerRegister();
            ASSERT_require(!REG_ESP.isEmpty());
            BaseSemantics::SValuePtr esp = dispatcher->get_operators()->peekRegister(REG_ESP, dispatcher->undefined_(32));
            BaseSemantics::SValuePtr arg0addr = dispatcher->get_operators()->add(esp, dispatcher->number_(32, 4));
            BaseSemantics::SValuePtr arg0 = dispatcher->get_operators()->peekMemory(RegisterDescriptor(),
                                                                                        arg0addr, dispatcher->undefined_(32));
            if (arg0->is_number()) {
                // The address of "main" is passed as the first argument.
                SAWYER_MESG(debug) <<"LibcStartMain analysis: x86 with main as the first argument\n";
                mainVa_ = arg0->get_number();
                functionPtrs.push_back(arg0);
            }

        } else {
            // architecture not supported yet
        }
    }

    if (!functionPtrs.empty()) {
        ASSERT_require(args.bblock->successors().isCached());
        BasicBlock::Successors succs = args.bblock->successors().get();
        BOOST_FOREACH (const BaseSemantics::SValuePtr &calleeVa, functionPtrs) {
            succs.push_back(BasicBlock::Successor(Semantics::SValue::promote(calleeVa), E_FUNCTION_CALL));
            SAWYER_MESG(debug) <<"LibcStartMain analysis: fcall to " <<*calleeVa
                               <<(mainVa_ && calleeVa->get_number() == *mainVa_ ? ", assumed to be \"main\"" : "") <<"\n";
        }
        args.bblock->successors() = succs;
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
