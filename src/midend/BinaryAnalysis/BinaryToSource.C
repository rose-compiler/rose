#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>

#include <boost/algorithm/string/replace.hpp>
#include <AsmUnparser_compat.h>
#include <BinaryToSource.h>
#include <CommandLine.h>

using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

typedef SourceAstSemantics::SValue SValue;
typedef SourceAstSemantics::SValuePtr SValuePtr;
typedef SourceAstSemantics::RegisterState RegisterState;
typedef SourceAstSemantics::RegisterStatePtr RegisterStatePtr;
typedef SourceAstSemantics::MemoryState MemoryState;
typedef SourceAstSemantics::MemoryStatePtr MemoryStatePtr;
typedef SourceAstSemantics::State State;
typedef SourceAstSemantics::StatePtr StatePtr;
typedef SourceAstSemantics::RiscOperators RiscOperators;
typedef SourceAstSemantics::RiscOperatorsPtr RiscOperatorsPtr;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility BinaryToSource::mlog;

// class method
void
BinaryToSource::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("Rose::BinaryAnalysis::BinaryToSource", Diagnostics::destination);
        mlog.comment("lifting assembly to C");
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
BinaryToSource::init(const P2::Partitioner &partitioner) {
    disassembler_ = partitioner.instructionProvider().disassembler();
    const RegisterDictionary *regDict = disassembler_->registerDictionary();
    raisingOps_ = RiscOperators::instance(regDict, SmtSolverPtr());
    BaseSemantics::DispatcherPtr protoCpu = disassembler_->dispatcher();
    if (!protoCpu)
        throw Exception("no instruction semantics for architecture");
    if (settings_.traceRiscOps) {
        tracingOps_ = TraceSemantics::RiscOperators::instance(raisingOps_);
        raisingCpu_ = protoCpu->create(tracingOps_);
    } else {
        raisingCpu_ = protoCpu->create(raisingOps_);
    }
}

// class method
Sawyer::CommandLine::SwitchGroup
BinaryToSource::commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("Lifter switches");
    sg.name("lifter");
    sg.doc("These switches apply to the BinaryToSource analysis, which produces low-level C source code from machine instructions "
           "using ROSE's instruction semantics.");

    Rose::CommandLine::insertBooleanSwitch(sg, "trace-generation", settings.traceRiscOps,
                                           "Cause the source generation phase to emit information about the basic RISC-like steps "
                                           "performed for each instruction. This can preserve a developer's sanity because the C "
                                           "expressions often become large, deeply nested, and not always intuitive about from "
                                           "whence each part came.");

    Rose::CommandLine::insertBooleanSwitch(sg, "trace-instructions", settings.traceInsnExecution,
                                           "Cause the generated source to contain extra \"printf\" calls to emit each instruction "
                                           "as it is processed.");

    sg.insert(Switch("ip")
              .longName("instruction-pointer")
              .argument("address", nonNegativeIntegerParser(settings.initialInstructionPointer))
              .doc("Initial value for the instruction pointer. If no initial value is provided and a function with the name "
                   "\"_start\" is present, then use that function as the starting point. Otherwise the instruction pointer "
                   "reigster is not given an initial value and the generated C program will probably fail to run (also an "
                   "error is emitted during the generation of the C program). The default is " +
                   (settings.initialInstructionPointer ?
                    StringUtility::addrToString(*settings.initialInstructionPointer) :
                    std::string("no value")) + "."));

    sg.insert(Switch("sp")
              .longName("stack-pointer")
              .argument("address", nonNegativeIntegerParser(settings.initialStackPointer))
              .doc("Initial value for the stack pointer. If no value is specified then the stack pointer register is not "
                   "initialized. The default is " +
                   (settings.initialStackPointer ?
                    StringUtility::addrToString(*settings.initialStackPointer) :
                    std::string("no value")) + "."));


    sg.insert(Switch("allocate-memory")
              .argument("size", nonNegativeIntegerParser(settings.allocateMemoryArray))
              .doc("Causes the global \"mem\" array to be allocated instead of being declared \"extern\". The switch "
                   "argument is the amount of memory to allocate. If the argument is zero, then the memory array is "
                   "allocated to be just large enough to hold the value at the maximum initialized address. The default is to " +
                   (settings.allocateMemoryArray ?
                    (0 == *settings.allocateMemoryArray ?
                     "allocate to the maximum initialized address" :
                     "allocate " + StringUtility::plural(*settings.allocateMemoryArray, "bytes")) :
                    std::string("not allocate anything")) + "."));

    Rose::CommandLine::insertBooleanSwitch(sg, "zero-memory", settings.zeroMemoryArray,
                                           "Cause the allocated memory array to be explicitly cleared before it is initialized "
                                           "with the contents of the executable. If disabled, then the memory array is allocated "
                                           "with \"malloc\" instead of \"calloc\", which is faster and might be able to handle "
                                           "much larger memory sizes.");

    return sg;
}

void
BinaryToSource::emitFilePrologue(const P2::Partitioner &partitioner, std::ostream &out) {
    out <<"#include <assert.h>\n"
        <<"#include <signal.h>\n"
        <<"#include <stdint.h>\n"
        <<"#include <stdio.h>\n"
        <<"#include <stdlib.h>\n"
        <<"#include <sys/types.h>\n"
        <<"#include <unistd.h>\n"
        <<"\n"
        <<"void function_call(void);\n"
        <<"void hlt(void) __attribute__((noreturn));\n"
        <<"void initialize_memory(void);\n"
        <<"void interrupt(int majr, int minr);\n"
        <<"void segfault(void) __attribute__((noreturn));\n"
        <<"unsigned unspecified(void);\n"
        <<"\n";

    out <<"/* Memory */\n";
    if (!settings_.allocateMemoryArray) {
        out <<"extern uint8_t *mem;\n";
    } else {
        out <<"uint8_t *mem = NULL;\n";
    }

    out <<"\n"
        <<"/* Called for x86 HLT instruction */\n"
        <<"void hlt(void) {\n"
        <<"    kill(getpid(), SIGILL);\n"
        <<"    abort();\n"
        <<"}\n"
        <<"\n"
        <<"/* Called when we lose track of the instruction pointer */\n"
        <<"void segfault(void) {\n"
        <<"    kill(getpid(), SIGSEGV);\n"
        <<"    abort();\n"
        <<"}\n"
        <<"\n"
        <<"/* Generates a value for unspecified behavior. */\n"
        <<"unsigned\n"
        <<"unspecified(void) {\n"
        <<"    int x;\n"
        <<"    return x; /* intentionally uninitialized */\n"
        <<"}\n"
        <<"\n";
}

void
BinaryToSource::declareGlobalRegisters(std::ostream &out) {
    out <<"\n/* Global register variables */\n";
    RegisterStatePtr regs = RegisterState::promote(raisingOps_->currentState()->registerState());
    BOOST_FOREACH (const RegisterState::RegPair &regpair, regs->get_stored_registers()) {
        std::string ctext = SValue::unsignedTypeNameForSize(regpair.desc.nBits()) + " " +
                            raisingOps_->registerVariableName(regpair.desc);
        if (regpair.desc.nBits() > 64) {
            out <<"/* " <<ctext <<"; -- not supported yet in ROSE source analysis */\n";
        } else {
            out <<ctext <<";\n";
        }
    }
    out <<"\n";
}

void
BinaryToSource::defineInterrupts(std::ostream &out) {
    out <<"\n"
        <<"void interrupt(int majr, int minr) {\n";

    if (disassembler_->instructionPointerRegister().nBits() == 32) {
        out <<"    if (0 == majr && 0x80 == minr && 1 == R_eax) {\n"
            <<"        fprintf(stderr, \"exited with status %d\", R_ebx);\n"
            <<"        exit(R_ebx);\n";
    } else {
        out <<"    if (0) {\n"
            <<"        /* no special interrupts for this architecture */\n";
    }

    out <<"    } else {\n"
        <<"        printf(\"interrupt(%d, %d)\\n\", majr, minr);\n"
        <<"    }\n"
        <<"}\n";
}

void
BinaryToSource::emitEffects(std::ostream &out) {
    out <<"                    /* SSA */\n";
    BOOST_FOREACH (const RiscOperators::SideEffect &sideEffect, raisingOps_->sideEffects()) {
        std::string value = SValue::promote(sideEffect.expression)->ctext();
        if (sideEffect.temporary) {
            std::string tempType = SValue::unsignedTypeNameForSize(sideEffect.expression->get_width());
            std::string tempName = SValue::promote(sideEffect.temporary)->ctext();
            out <<"                    " <<tempType <<" " <<tempName <<" = " <<value <<";\n";
        } else {
            out <<"                    " <<value <<";\n";
        }
    }
    out <<"                    /* Side effects */\n";

    // Show last occurrence of each side effect.
    const std::vector<RiscOperators::SideEffect> &sideEffects = raisingOps_->sideEffects();
    for (size_t i = 0; i < sideEffects.size(); ++i) {
        if (sideEffects[i].location) {
            std::string location = SValue::promote(sideEffects[i].location)->ctext();
            bool isLastOccurrence = true;
            for (size_t j = i+1; isLastOccurrence && j < sideEffects.size(); ++j) {
                if (sideEffects[j].location) {
                    std::string laterLocation = SValue::promote(sideEffects[j].location)->ctext();
                    if (location == laterLocation)
                        isLastOccurrence = false;
                }
            }
            if (isLastOccurrence) {
                std::string tempName = SValue::promote(sideEffects[i].temporary)->ctext();
                out <<"                    " <<location <<" = " <<tempName <<";\n";
            }
        }
    }
}

void
BinaryToSource::emitInstruction(SgAsmInstruction *insn, std::ostream &out) {
    ASSERT_not_null(insn);
    out <<"                /* "<<unparseInstruction(insn) <<" */\n";
    if (settings_.traceInsnExecution)
        out <<"                fputs("
            <<"\"" <<StringUtility::cEscape(insn->toString()) <<"\\n\""
            <<", stderr);\n";

    raisingOps_->reset();
    try {
        raisingCpu_->processInstruction(insn);
    } catch (const BaseSemantics::Exception &e) {
        out <<"                fputs(\"semantics exception: " <<StringUtility::cEscape(e.what()) <<"\", stderr);\n";
    }
    out <<"                {\n";
    emitEffects(out);
    out <<"                }\n";
}

void
BinaryToSource::emitBasicBlock(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bblock, std::ostream &out) {
    out <<"            case " <<StringUtility::addrToString(bblock->address()) <<":\n";

    rose_addr_t fallThroughVa = 0;
    raisingOps_->resetState();
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        emitInstruction(insn, out);
        fallThroughVa = insn->get_address() + insn->get_size();
    }

    // If this bblock is a binary function call, then call the corresponding source function.  We can't do this
    // directly because the call might be indirect. Therefore all calls go through a function call dispatcher.
    if (partitioner.basicBlockIsFunctionCall(bblock))
        out <<"                function_call();\n";

    bool needBreak = true;
    P2::ControlFlowGraph::ConstVertexIterator placeholder = partitioner.findPlaceholder(bblock->address());
    ASSERT_require(partitioner.cfg().isValidVertex(placeholder));
    if (placeholder->nOutEdges() == 0) {
        out <<"                assert(!\"not reachable\");\n";
        needBreak = false;
    } else if (placeholder->nOutEdges() == 1) {
        P2::ControlFlowGraph::ConstVertexIterator nextVertex = placeholder->outEdges().begin()->target();
        if (nextVertex->value().type() == P2::V_BASIC_BLOCK && nextVertex->value().address() == fallThroughVa) {
            out <<"                /* fall through... */\n";
            needBreak = false;
        }
    }
    if (needBreak)
        out <<"                break;\n";
}

void
BinaryToSource::emitFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function, std::ostream &out) {
    std::string name = function->name();
    name = boost::replace_all_copy(name, "*/", "*\\/");
    out <<"\n";
    if (!name.empty())
        out <<"/* " <<name <<" */\n";

    const RegisterDescriptor IP = disassembler_->instructionPointerRegister();
    out <<"void F_" <<StringUtility::addrToString(function->address()).substr(2) <<"("
        <<"const " <<SValue::unsignedTypeNameForSize(IP.nBits()) <<" ret_va"
        <<") {\n"
        <<"    while (" <<raisingOps_->registerVariableName(IP) <<" != ret_va) {\n"
        <<"        switch (" <<raisingOps_->registerVariableName(IP) <<") {\n";
    BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
        P2::ControlFlowGraph::ConstVertexIterator placeholder = partitioner.findPlaceholder(bblockVa);
        ASSERT_require(partitioner.cfg().isValidVertex(placeholder));
        ASSERT_require(placeholder->value().type() == P2::V_BASIC_BLOCK);
        P2::BasicBlock::Ptr bblock = placeholder->value().bblock();
        emitBasicBlock(partitioner, bblock, out);
    }
    out <<"            default:\n"
        <<"                segfault();\n"
        <<"        }\n"
        <<"    }\n"
        <<"}\n";
}

void
BinaryToSource::emitAllFunctions(const P2::Partitioner &partitioner, std::ostream &out) {
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions())
        emitFunction(partitioner, function, out);
}

void
BinaryToSource::emitFunctionDispatcher(const P2::Partitioner &partitioner, std::ostream &out) {
    out <<"\n"
        <<"void\n"
        <<"function_call(void) {\n";

    // To whence does this call return?
    const RegisterDescriptor IP = disassembler_->instructionPointerRegister();
    const RegisterDescriptor SP = disassembler_->stackPointerRegister();
    const RegisterDescriptor SS = disassembler_->stackSegmentRegister();
    raisingOps_->reset();
    BaseSemantics::SValuePtr spDflt = raisingOps_->undefined_(SP.nBits());
    BaseSemantics::SValuePtr returnTarget = raisingOps_->readMemory(SS,
                                                                    raisingOps_->peekRegister(SP, spDflt),
                                                                    raisingOps_->undefined_(IP.nBits()),
                                                                    raisingOps_->boolean_(true));
    emitEffects(out);
    out <<"    " <<SValue::unsignedTypeNameForSize(IP.nBits()) <<" returnTarget = "
        <<SValue::promote(returnTarget)->ctext() <<";\n";

    // Emit the dispatch table
    out <<"    switch (" <<raisingOps_->registerVariableName(IP) <<") {\n";
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        out <<"        case " <<StringUtility::addrToString(function->address()) <<": ";
        out <<"            F_" <<StringUtility::addrToString(function->address()).substr(2) <<"(returnTarget); ";
        out <<"            break;";
        std::string fname = function->name();
        if (!fname.empty()) {
            fname = StringUtility::cEscape(fname);
            fname = boost::replace_all_copy(fname, "*/", "*\\/");
            out <<" /* " <<fname <<" */";
        }
        out <<"\n";
    }
    out <<"    }\n"
        <<"}\n";
}

void
BinaryToSource::emitMemoryInitialization(const P2::Partitioner &partitioner, std::ostream &out) {
    out <<"\n"
        <<"void\n"
        <<"initialize_memory(void) {\n";

    // Allocate the memory array
    if (settings_.allocateMemoryArray) {
        if (0 == *settings_.allocateMemoryArray) {
            if (settings_.zeroMemoryArray) {
                out <<"    mem = calloc(" <<StringUtility::addrToString(partitioner.memoryMap()->greatest()+1) <<"UL, 1);\n";
            } else {
                out <<"    mem = malloc(" <<StringUtility::addrToString(partitioner.memoryMap()->greatest()+1) <<"UL);\n";
            }
        } else {
            if (settings_.zeroMemoryArray) {
                out <<"    mem = calloc(" <<StringUtility::addrToString(*settings_.allocateMemoryArray) <<"UL, 1);\n";
            } else {
                out <<"    mem = malloc(" <<StringUtility::addrToString(*settings_.allocateMemoryArray) <<"UL);\n";
            }
        }
    }

    // Initialize the memory array with the contents of the memory map
    rose_addr_t va = 0;
    uint8_t buf[8192];
    while (AddressInterval where = partitioner.memoryMap()->atOrAfter(va).limit(sizeof buf).read(buf)) {
        uint8_t *bufptr = buf;
        for (va = where.least(); va <= where.greatest(); ++va, ++bufptr) {
            out <<"    mem[" <<StringUtility::addrToString(va) <<"]"
                <<"= " <<StringUtility::toHex2(*bufptr, 8, false, false) <<";\n";
        }
        if (va <= partitioner.memoryMap()->hull().least())
            break;                                      // overflow of ++va
    }

    out <<"}\n";
}

void
BinaryToSource::emitMain(const P2::Partitioner &partitioner, std::ostream &out) {
    out <<"\n"
        <<"int\n"
        <<"main() {\n"
        <<"    initialize_memory();\n";

    // Initialize regsiters
    Sawyer::Optional<rose_addr_t> initialIp;
    if (settings_.initialInstructionPointer) {
        initialIp = *settings_.initialInstructionPointer;
    } else {
        BOOST_FOREACH (const P2::Function::Ptr &f, partitioner.functions()) {
            if (f->name() == "_start") {
                initialIp = f->address();
                break;
            }
        }
    }
    if (!initialIp) {
        mlog[ERROR] <<"no initial value specified for instruction pointer register, an no \"_start\" function found\n";
    } else {
        const RegisterDescriptor reg = disassembler_->instructionPointerRegister();
        SValuePtr val = SValue::promote(raisingOps_->number_(reg.nBits(), *initialIp));
        out <<"    " <<raisingOps_->registerVariableName(reg) <<" = " <<val->ctext() <<";\n";
    }

    if (settings_.initialStackPointer) {
        const RegisterDescriptor reg = disassembler_->stackPointerRegister();
        SValuePtr val = SValue::promote(raisingOps_->number_(reg.nBits(), *settings_.initialStackPointer));
        out <<"    " <<raisingOps_->registerVariableName(reg) <<" = " <<val->ctext() <<";\n";
    }

    // Initialize call frame
    {
        static const rose_addr_t magic = 0xfffffffffffffeull ; // arbitrary
        size_t bytesPerWord = disassembler_->wordSizeBytes();
        std::string sp = raisingOps_->registerVariableName(disassembler_->stackPointerRegister());
        for (size_t i=0; i<bytesPerWord; ++i)
            out <<"    mem[--" <<sp <<"] = " <<((magic>>(8*i)) & 0xff) <<"; /* arbitrary */\n";
    }
    out <<"    function_call();\n"
        <<"    return 0;\n"
        <<"}\n";
}

void
BinaryToSource::generateSource(const P2::Partitioner &partitioner, std::ostream &out) {
    init(partitioner);
    emitFilePrologue(partitioner, out);
    declareGlobalRegisters(out);
    defineInterrupts(out);
    emitAllFunctions(partitioner, out);
    emitFunctionDispatcher(partitioner, out);
    emitMemoryInitialization(partitioner, out);
    emitMain(partitioner, out);
}

} // namespace
} // namespace

#endif
