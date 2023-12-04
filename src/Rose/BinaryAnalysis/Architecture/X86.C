#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/X86.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
registerPrefix(size_t bitsPerWord) {
    switch (bitsPerWord) {
        case 16:
            return "";
        case 32:
            return "e";
        case 64:
            return "r";
    }
    ASSERT_not_reachable("invalid number of bits per word");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

X86::X86(const std::string &name, size_t bytesPerWord)
    : Base(name, bytesPerWord, ByteOrder::ORDER_LSB) {}

X86::~X86() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction characteristics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
X86::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case x86_unknown_instruction:
        case x86_call:
        case x86_ret:
        case x86_iret:
        case x86_farcall:
        case x86_farjmp:
        case x86_hlt:
        case x86_jmp:
        case x86_int:
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_ja:
        case x86_jae:
        case x86_jb:
        case x86_jbe:
        case x86_jcxz:
        case x86_je:
        case x86_jecxz:
        case x86_jg:
        case x86_jge:
        case x86_jl:
        case x86_jle:
        case x86_jne:
        case x86_jno:
        case x86_jns:
        case x86_jo:
        case x86_jpe:
        case x86_jpo:
        case x86_jrcxz:
        case x86_js:
        case x86_loop:
        case x86_loopnz:
        case x86_loopz:
        case x86_retf:
        case x86_rsm:
        case x86_syscall:
        case x86_sysret:
        case x86_ud2:
            return true;
        default: return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Factories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Disassembler::Base::Ptr
X86::newInstructionDecoder() const {
    return Disassembler::X86::instance(shared_from_this());
}

Unparser::Base::Ptr
X86::newUnparser() const {
    return Unparser::X86::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
X86::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherX86::instance(shared_from_this(), ops);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner matchers and callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
X86::functionPrologueMatchers(const Partitioner2::Engine::Ptr &engine) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesX86::MatchHotPatchPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchStandardPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchAbbreviatedPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchEnterPrologue::instance());
    if (engine->settings().partitioner.findingThunks) {
        if (auto engineBinary = engine.dynamicCast<Partitioner2::EngineBinary>())
            retval.push_back(Partitioner2::Modules::MatchThunk::instance(engineBinary->functionMatcherThunks()));
    }
    retval.push_back(Partitioner2::ModulesX86::MatchRetPadPush::instance());
    return retval;
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
X86::basicBlockCreationHooks(const Partitioner2::Engine::Ptr&) const {
    std::vector<Partitioner2::BasicBlockCallback::Ptr> retval;
    retval.push_back(Partitioner2::ModulesX86::FunctionReturnDetector::instance());
    retval.push_back(Partitioner2::ModulesX86::SwitchSuccessors::instance());
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calling conventions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CallingConvention::Definition::Ptr
X86::cc_cdecl(size_t bitsPerWord) const {
    // Register prefix letter for things like "ax", "eax", "rax"
    const std::string prefix = registerPrefix(bitsPerWord);
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "cdecl",
                                                      "x86-" + boost::lexical_cast<std::string>(bitsPerWord) + " cdecl",
                                                      regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bitsPerWord >> 3);        // return address

    //==== Function parameters ====
    // All parameters are passed on the stack.
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

    //==== Other inputs ====
    // direction flag is always assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow(prefix + "ax"));
    if (const RegisterDescriptor ST0 = regdict->findLargestRegister(x86_regclass_st, x86_st_0))
        cc->appendOutputParameter(ST0);
    cc->appendOutputParameter(SP);                      // final value is usually one word greater than initial value

    //==== Scratch registers ====
    // (i.e., modified, not callee-saved, not return registers)
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "cx"));
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "dx"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "flags"));
    if (const RegisterDescriptor FPSTATUS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus))
        cc->scratchRegisters().insert(FPSTATUS);

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

CallingConvention::Definition::Ptr
X86::cc_stdcall(size_t bitsPerWord) const {
    const std::string prefix = registerPrefix(bitsPerWord);
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "stdcall",
                                                      "x86-" + boost::lexical_cast<std::string>(bitsPerWord) + " stdcall",
                                                      regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bitsPerWord >> 3);        // return address

    //==== Function parameters ====
    // All parameters are passed on the stack
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLEE);

    //==== Other inputs ====
    // direction flag is always assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow(prefix + "ax"));
    if (const RegisterDescriptor ST0 = regdict->findLargestRegister(x86_regclass_st, x86_st_0))
        cc->appendOutputParameter(ST0);
    cc->appendOutputParameter(SP);

    //==== Scratch registers ====
    // Modified, not callee-saved, not return registers
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "cx"));
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "dx"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "flags"));
    if (const RegisterDescriptor FPSTATUS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus))
        cc->scratchRegisters().insert(FPSTATUS);

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

CallingConvention::Definition::Ptr
X86::cc_fastcall(size_t bitsPerWord) const {
    const std::string prefix = registerPrefix(bitsPerWord);
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "fastcall",
                                                      "x86-" + boost::lexical_cast<std::string>(bitsPerWord) + " fastcall",
                                                      regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bitsPerWord >> 3);        // return address

    //==== Function parameters ====
    // Uses ECX and EDX for first args that fit; all other parameters are passed on the stack.
    cc->appendInputParameter(regdict->findOrThrow(prefix + "cx"));
    cc->appendInputParameter(regdict->findOrThrow(prefix + "dx"));
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLEE);

    //==== Other inputs ====
    // direction flag is always assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow(prefix + "ax"));
    if (const auto ST0 = regdict->findLargestRegister(x86_regclass_st, x86_st_0))
        cc->appendOutputParameter(ST0);
    cc->appendOutputParameter(SP);

    //==== Scratch registers ====
    // I.e., modified, not callee-saved, not return registers
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "cx"));
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "dx"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "flags"));
    if (const auto FPSTATUS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus))
        cc->scratchRegisters().insert(FPSTATUS);

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

const CallingConvention::Dictionary&
X86::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CallingConvention::Dictionary dict;

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        {
            auto cc = cc_cdecl(bitsPerWord());
            cc->comment(cc.comment() + " 4-byte alignment");
            cc->stackAlignment(4);
            dict.push_back(cc);
        }

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        {
            auto cc = cc_cdecl(bitsPerWord());
            cc->comment(cc.comment() + " 16-byte alignment");
            cc->stackAlignment(16);
            dict.push_back(cc);
        }
#else
        dict.push_back(cc_cdecl(bitsPerWord()));
#endif

        // other conventions
        dict.push_back(cc_stdcall(bitsPerWord()));
        dict.push_back(cc_fastcall(bitsPerWord()));

        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

} // namespace
} // namespace
} // namespace

#endif
