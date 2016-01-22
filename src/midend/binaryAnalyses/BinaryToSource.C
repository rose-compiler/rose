#include <sage3basic.h>

#include <boost/algorithm/string/replace.hpp>
#include <AsmUnparser_compat.h>
#include <BinaryToSource.h>

using namespace rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

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

namespace rose {
namespace BinaryAnalysis {

void
BinaryToSource::init(const P2::Partitioner &partitioner) {
    disassembler_ = partitioner.instructionProvider().disassembler();
    const RegisterDictionary *regDict = disassembler_->get_registers();
    raisingOps_ = RiscOperators::instance(regDict, NULL);
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
    } else if (0 == *settings_.allocateMemoryArray) {
        out <<"uint8_t mem[" <<StringUtility::addrToString(partitioner.memoryMap().greatest()+1) <<"];\n";
    } else {
        out <<"uint8_t mem[" <<StringUtility::addrToString(*settings_.allocateMemoryArray) <<"];\n";
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
        <<"\n"
        <<"void\n"
        <<"interrupt(int majr, int minr) {\n"
        <<"    printf(\"interrupt(%d,%d)\\n\", majr, minr);\n"
        <<"}\n"
        <<"\n";
}

void
BinaryToSource::declareGlobalRegisters(std::ostream &out) {
    out <<"\n/* Global register variables */\n";
    RegisterStatePtr regs = RegisterState::promote(raisingOps_->currentState()->get_register_state());
    BOOST_FOREACH (const RegisterState::RegPair &regpair, regs->get_stored_registers()) {
        std::string ctext = SValue::unsignedTypeNameForSize(regpair.desc.get_nbits()) + " " +
                            raisingOps_->registerVariableName(regpair.desc);
        if (regpair.desc.get_nbits() > 64) {
            out <<"/* " <<ctext <<"; -- not supported yet in ROSE source analysis */\n";
        } else {
            out <<ctext <<";\n";
        }
    }
    out <<"\n";
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
    BOOST_FOREACH (const RiscOperators::SideEffect &sideEffect, raisingOps_->sideEffects()) {
        if (sideEffect.location) {
            std::string location = SValue::promote(sideEffect.location)->ctext();
            std::string tempName = SValue::promote(sideEffect.temporary)->ctext();
            out <<"                    " <<location <<" = " <<tempName <<";\n";
        }
    }
}

void
BinaryToSource::emitInstruction(SgAsmInstruction *insn, std::ostream &out) {
    ASSERT_not_null(insn);
    out <<"                /* "<<unparseInstruction(insn) <<" */\n";
    if (settings_.traceInsnExecution)
        out <<"                fputs("
            <<"\"" <<StringUtility::cEscape(unparseInstructionWithAddress(insn)) <<"\\n\""
            <<", stderr);\n";

    raisingOps_->reset();
    raisingCpu_->processInstruction(insn);
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
    const RegisterDescriptor IP = disassembler_->instructionPointerRegister();
    out <<"\nvoid F_" <<StringUtility::addrToString(function->address()).substr(2) <<"("
        <<"const " <<SValue::unsignedTypeNameForSize(IP.get_nbits()) <<" ret_va"
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
    BaseSemantics::SValuePtr returnTarget = raisingOps_->readMemory(SS, raisingOps_->readRegister(SP),
                                                                    raisingOps_->undefined_(IP.get_nbits()),
                                                                    raisingOps_->boolean_(true));
    emitEffects(out);
    out <<"    " <<SValue::unsignedTypeNameForSize(IP.get_nbits()) <<" returnTarget = "
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
            boost::replace_all(fname, "*/", "*\\/");
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
    rose_addr_t va = 0;
    uint8_t buf[8192];
    while (AddressInterval where = partitioner.memoryMap().atOrAfter(va).limit(sizeof buf).read(buf)) {
        uint8_t *bufptr = buf;
        for (va = where.least(); va <= where.greatest(); ++va, ++bufptr) {
            out <<"    mem[" <<StringUtility::addrToString(va) <<"]"
                <<"= " <<StringUtility::toHex2(*bufptr, 8, false, false) <<";\n";
        }
        if (va <= partitioner.memoryMap().hull().least())
            break;                                      // overflow of ++va
    }
    out <<"}\n";
}

void
BinaryToSource::emitMain(std::ostream &out) {
    out <<"\n"
        <<"int\n"
        <<"main() {\n"
        <<"    initialize_memory();\n";

    // Initialize regsiters
    if (settings_.initialInstructionPointer) {
        const RegisterDescriptor reg = disassembler_->instructionPointerRegister();
        SValuePtr val = SValue::promote(raisingOps_->number_(reg.get_nbits(), *settings_.initialInstructionPointer));
        out <<"    " <<raisingOps_->registerVariableName(reg) <<" = " <<val->ctext() <<";\n";
    }

    if (settings_.initialStackPointer) {
        const RegisterDescriptor reg = disassembler_->stackPointerRegister();
        SValuePtr val = SValue::promote(raisingOps_->number_(reg.get_nbits(), *settings_.initialStackPointer));
        out <<"    " <<raisingOps_->registerVariableName(reg) <<" = " <<val->ctext() <<";\n";
    }

    // Initialize call frame
    {
        static const rose_addr_t magic = 0xfffffffffffffeull ; // arbitrary
        size_t bytesPerWord = disassembler_->get_wordsize();
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
    emitAllFunctions(partitioner, out);
    emitFunctionDispatcher(partitioner, out);
    emitMemoryInitialization(partitioner, out);
    emitMain(out);
}

    

} // namespace
} // namespace
