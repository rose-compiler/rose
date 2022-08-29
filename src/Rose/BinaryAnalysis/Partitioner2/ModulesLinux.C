#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesLinux.h>

#include <Rose/BinaryAnalysis/SystemCall.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesLinux {

SystemCall
systemCallAnalyzer(const Partitioner &partitioner, const boost::filesystem::path &syscallHeader) {
    SystemCall analyzer;

    // We only support x86 Linux (32- or 64-bit)
    if (partitioner.instructionProvider().disassembler().dynamicCast<Disassembler::X86>()) {
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
    for (const BasicBlock::Ptr &bb: partitioner.basicBlocks()) {
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

BaseSemantics::SValuePtr
LibcStartMain::readStack(const Partitioner &partitioner, const BaseSemantics::DispatcherPtr &cpu, int byteOffset,
                         size_t nBits, RegisterDescriptor segmentRegister) {
    const RegisterDescriptor SP = partitioner.instructionProvider().stackPointerRegister();
    if (SP.isEmpty())
        return BaseSemantics::SValuePtr();
    BaseSemantics::SValuePtr sp = cpu->operators()->peekRegister(SP, cpu->undefined_(SP.nBits()));
    BaseSemantics::SValuePtr stackOffset = cpu->number_(SP.nBits(),
                                                        BitOps::signExtend((uint64_t)byteOffset, 8*sizeof(byteOffset)));
    BaseSemantics::SValuePtr addr = cpu->operators()->add(sp, stackOffset);
    BaseSemantics::SValuePtr value = cpu->operators()->peekMemory(segmentRegister, addr, cpu->undefined_(nBits));
    return value;
}

bool
LibcStartMain::operator()(bool chain, const Args &args) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Look at this block only if it's a function call
    if (!chain || !args.bblock || args.bblock->nInstructions() == 0)
        return chain;
    if (!args.partitioner.basicBlockIsFunctionCall(args.bblock))
        return chain;

    // It must call a function named "__libc_start_main@plt"
    bool foundCallToLibcStartMain = false;
    for (const rose_addr_t &succVa: args.partitioner.basicBlockConcreteSuccessors(args.bblock)) {
        Function::Ptr func = args.partitioner.functionExists(succVa);
        if (func && (func->name() == "__libc_start_main@plt" || func->name() == "__libc_start_main")) {
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
                for (SgAsmInstruction *insn: args.bblock->instructions())
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
    RegisterDictionary::Ptr regs = args.partitioner.instructionProvider().registerDictionary();
    if (isSgAsmX86Instruction(args.bblock->instructions().back())) {
        if (dispatcher->addressWidth() == 64) {
            // Amd64 integer arguments are passed in registers: rdi, rsi, rdx, rcx, r8, and r9
            const RegisterDescriptor FIRST_ARG = regs->findOrThrow("rdi");
            const RegisterDescriptor FOURTH_ARG = regs->findOrThrow("rcx");
            const RegisterDescriptor FIFTH_ARG = regs->findOrThrow("r8");
            BaseSemantics::SValuePtr firstArg = state->peekRegister(FIRST_ARG, dispatcher->undefined_(64), ops.get());
            BaseSemantics::SValuePtr fourthArg = state->peekRegister(FOURTH_ARG, dispatcher->undefined_(64), ops.get());
            BaseSemantics::SValuePtr fifthArg = state->peekRegister(FIFTH_ARG, dispatcher->undefined_(64), ops.get());

            if (firstArg->isConcrete() && fourthArg->isConcrete() && fifthArg->isConcrete() &&
                args.partitioner.memoryMap()->at(firstArg->toUnsigned().get()).require(MemoryMap::EXECUTABLE).exists() &&
                args.partitioner.memoryMap()->at(fourthArg->toUnsigned().get()).require(MemoryMap::EXECUTABLE).exists() &&
                args.partitioner.memoryMap()->at(fifthArg->toUnsigned().get()).require(MemoryMap::EXECUTABLE).exists()) {
                // Sometimes the address of main is passed as the first argument (rdi) with __libc_csu_init and __libc_csu_fini
                // passed as the fourth (rcx) and fifth (r8) arguments. By this point in the disassembly process, would not
                // have discovered PLT function yet. So if the first, fourth, and fifth arguments seem to point at executable
                // memory then assume they are "main", "__libc_csu_fini@plt", and "libc_csu_init@plt". Don't bother naming the
                // two PLT functions though since we'll get their names later by processing the PLT.
                SAWYER_MESG(debug) <<"LibcStartMain analysis: amd64 with main as the fisrt argument\n";
                mainVa_ = firstArg->toUnsigned().get();
                functionPtrs.push_back(firstArg);
                functionPtrs.push_back(fourthArg);
                functionPtrs.push_back(fifthArg);

            } else if (fourthArg->toUnsigned() &&
                       args.partitioner.memoryMap()->at(fourthArg->toUnsigned().get()).require(MemoryMap::EXECUTABLE).exists()) {
                // Sometimes then address of main is passed as the fourth argument (in rcx).
                SAWYER_MESG(debug) <<"LibcStartMain analysis: amd64 with main as the fourth argument\n";
                mainVa_ = fourthArg->toUnsigned().get();
                functionPtrs.push_back(fourthArg);
            }

        } else if (dispatcher->addressWidth() == 32) {
            // x86 integer arguments are passed on the stack. The address of main is the first argument, which starts four bytes
            // into the stack because the return address has also been pushed onto the stack.
            BaseSemantics::SValuePtr arg0 = readStack(args.partitioner, dispatcher, 4, 32, RegisterDescriptor());
            if (arg0->toUnsigned().assignTo(mainVa_)) {
                SAWYER_MESG(debug) <<"LibcStartMain analysis: x86 with main as the first argument\n";
                functionPtrs.push_back(arg0);
            }
        }
    } else if (isSgAsmM68kInstruction(args.bblock->instructions().back())) {
        // M68k integer arguments are passed on the stack. The address of main is the first argument, which starts four bytes
        // into the stack because the return address has also been pushed onto the stack.
        BaseSemantics::SValuePtr arg0 = readStack(args.partitioner, dispatcher, 4, 32, RegisterDescriptor());
        if (arg0->toUnsigned().assignTo(mainVa_)) {
            SAWYER_MESG(debug) <<"LibcStartMain analysis: m68k with main as the first argument\n";
            functionPtrs.push_back(arg0);
        }
#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (isSgAsmAarch32Instruction(args.bblock->instructions().back())) {
        // The "main" pointer is passed to __libc_start_main@plt in the r0 register.
        const RegisterDescriptor REG_R0 = regs->findOrThrow("r0");
        BaseSemantics::SValuePtr r0 = state->peekRegister(REG_R0, dispatcher->undefined_(32), ops.get());
        if (r0->toUnsigned().assignTo(mainVa_)) {
            SAWYER_MESG(debug) <<"LibcStartMain analysis: AArch32 with main in r0\n";
            functionPtrs.push_back(r0);
        }
#endif
    }

    if (!functionPtrs.empty()) {
        ASSERT_require(args.bblock->successors().isCached());
        BasicBlock::Successors succs = args.bblock->successors().get();
        for (const BaseSemantics::SValuePtr &calleeVa: functionPtrs) {
            succs.push_back(BasicBlock::Successor(Semantics::SValue::promote(calleeVa), E_FUNCTION_CALL));
            SAWYER_MESG(debug) <<"LibcStartMain analysis: fcall to " <<*calleeVa
                               <<(mainVa_ && calleeVa->toUnsigned().get() == *mainVa_ ? ", assumed to be \"main\"" : "") <<"\n";
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
