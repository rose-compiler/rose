#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Amd64.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Amd64::Amd64()
    : X86("amd64", 8) {}

Amd64::~Amd64() {}

Amd64::Ptr
Amd64::instance() {
    return Ptr(new Amd64);
}

RegisterDictionary::Ptr
Amd64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-pentium4").orThrow()->registerDictionary());

        // Additional 64-bit (and hi-end 32-bit) registers
        regs->insert("rax", x86_regclass_gpr, x86_gpr_ax, 0, 64);
        regs->insert("rbx", x86_regclass_gpr, x86_gpr_bx, 0, 64);
        regs->insert("rcx", x86_regclass_gpr, x86_gpr_cx, 0, 64);
        regs->insert("rdx", x86_regclass_gpr, x86_gpr_dx, 0, 64);
        regs->insert("rsp", x86_regclass_gpr, x86_gpr_sp, 0, 64);
        regs->insert("rbp", x86_regclass_gpr, x86_gpr_bp, 0, 64);
        regs->insert("rsi", x86_regclass_gpr, x86_gpr_si, 0, 64);
        regs->insert("rdi", x86_regclass_gpr, x86_gpr_di, 0, 64);
        regs->insert("rip", x86_regclass_ip, 0, 0, 64);
        regs->insert("rflags", x86_regclass_flags, x86_flags_status, 0, 64);

        for (unsigned i=8; i<16; i++) {
            // New general purpose registers in various widths
            std::string name = "r" + StringUtility::numberToString(i);
            regs->insert(name,     x86_regclass_gpr, i, 0, 64);
            regs->insert(name+"b", x86_regclass_gpr, i, 0,  8);
            regs->insert(name+"w", x86_regclass_gpr, i, 0, 16);
            regs->insert(name+"d", x86_regclass_gpr, i, 0, 32);

            // New media XMM registers
            regs->insert(std::string("xmm")+StringUtility::numberToString(i),
                         x86_regclass_xmm, i, 0, 128);
        }

        // Additional flag bits with no official names
        for (unsigned i=32; i<64; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);

        // Control registers become 64 bits, and cr8 is added
        regs->resize("cr0", 64);
        regs->resize("cr1", 64);
        regs->resize("cr2", 64);
        regs->resize("cr3", 64);
        regs->resize("cr4", 64);
        regs->insert("cr8", x86_regclass_cr, 8, 0, 64);

        // Debug registers become 64 bits
        regs->resize("dr0", 64);
        regs->resize("dr1", 64);
        regs->resize("dr2", 64);
        regs->resize("dr3", 64);                        // dr4 and dr5 are reserved
        regs->resize("dr6", 64);
        regs->resize("dr7", 64);

        // Special registers
        regs->instructionPointerRegister("rip");
        regs->stackPointerRegister("rsp");
        regs->stackFrameRegister("rbp");
        regs->stackSegmentRegister("ss");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Amd64::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family;
}

const CallingConvention::Dictionary&
Amd64::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CallingConvention::Dictionary dict;

        //--------
        // 64-bit
        //--------

        // Listed first because it's the most common
        dict.push_back(cc_sysv());

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        {
            auto cc = cc_cdecl(64);
            cc->comment(cc.comment() + " 4-byte alignment");
            cc->stackAlignment(4);
            dict.push_back(cc);
        }

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        {
            auto cc = cc_cdecl(64);
            cc->comment(cc.comment() + " 16-byte alignment");
            cc->stackAlignment(16);
            dict.push_back(cc);
        }
#else
        dict.push_back(cc_cdecl(64));
#endif
        dict.push_back(cc_stdcall(64));

        //--------
        // 32-bit
        //--------

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        {
            auto cc = cc_cdecl(32);
            cc->comment(cc->comment() + " 4-byte alignment");
            cc->stackAlignment(4);
            dict.push_back(cc);
        }

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        {
            auto cc = cc_cdecl(32);
            cc->comment(cc->comment() + " 16-byte alignment");
            cc->stackAlignment(16);
            dict.push_back(cc);
        }
#else
        dict.push_back(cc_cdecl(32));
#endif

        // other conventions
        dict.push_back(cc_stdcall(32));
        dict.push_back(cc_fastcall(32));

        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

CallingConvention::Definition::Ptr
Amd64::cc_sysv() const {
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(64, "sysv", "x86-64 sysv", regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bytesPerWord());          // return address

    //==== Function parameters ====

    // The first six integer or pointer arguments are passed in registers RDI, RSI, RDX, RCX, R8, and R9.
    // These registers are also not preserved across the call.
    cc->appendInputParameter(regdict->findOrThrow("rdi"));
    cc->appendInputParameter(regdict->findOrThrow("rsi"));
    cc->appendInputParameter(regdict->findOrThrow("rdx"));
    cc->appendInputParameter(regdict->findOrThrow("rcx"));
    cc->appendInputParameter(regdict->findOrThrow("r8"));
    cc->appendInputParameter(regdict->findOrThrow("r9"));

    // The first eight SSE arguments are passed in registers xmm0 through xmm7
    cc->appendInputParameter(regdict->findOrThrow("xmm0"));
    cc->appendInputParameter(regdict->findOrThrow("xmm1"));
    cc->appendInputParameter(regdict->findOrThrow("xmm2"));
    cc->appendInputParameter(regdict->findOrThrow("xmm3"));
    cc->appendInputParameter(regdict->findOrThrow("xmm4"));
    cc->appendInputParameter(regdict->findOrThrow("xmm5"));
    cc->appendInputParameter(regdict->findOrThrow("xmm6"));
    cc->appendInputParameter(regdict->findOrThrow("xmm7"));

    // The AL register is an input register that stores the number of SSE registers used for variable argument calls. (It
    // is also part of the first return value).
    cc->appendInputParameter(regdict->findOrThrow("al")); // for varargs calls

    // Arguments that don't fit in the input registers are passed on the stack
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

    //==== Other inputs ====

    // direction flag is assuemd to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow("rax"));
    cc->appendOutputParameter(regdict->findOrThrow("rdx")); // second integer return
    cc->appendOutputParameter(SP);                          // final value is usually 8 greater than initial value
    cc->appendOutputParameter(regdict->findOrThrow("xmm0"));
    cc->appendOutputParameter(regdict->findOrThrow("xmm1"));
    //cc->scratchRegisters().insert(regdict->findLargestRegister(x86_regclass_st, 0)); // dynamic st(0), overlaps mm<i>
    //cc->scratchRegisters().insert(regdict->findLargestRegister(x86_regclass_st, 1)); // dynamic st(1), overlaps mm<i+1>

    //==== Scratch registers ====
    // Modified, not callee-saved, not return values

    // Registers that hold arguments are also scratch registers (as long as they're not return values)
    cc->scratchRegisters().insert(regdict->findOrThrow("rdi"));
    cc->scratchRegisters().insert(regdict->findOrThrow("rsi"));
    //cc->scratchRegisters().insert(regdict->findOrThrow("rdx")); this is a return reg
    cc->scratchRegisters().insert(regdict->findOrThrow("rcx"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r8"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r9"));

    //cc->scratchRegisters().insert(regdict->findLargestRegister(x86_regclass_xmm, 0)); this is a return reg
    //cc->scratchRegisters().insert(regdict->findLargestRegister(x86_regclass_xmm, 1)); this is a return reg
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm2"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm3"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm4"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm5"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm6"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm7"));

    // These registers are almost always modified by a function
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow("rflags"));
    cc->scratchRegisters().insert(regdict->findOrThrow("fpstatus"));

    cc->scratchRegisters().insert(regdict->findOrThrow("r10")); //static chain ptr
    cc->scratchRegisters().insert(regdict->findOrThrow("r11"));

    // Floating point registers are pretty much all scratch.
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm8"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm9"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm10"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm11"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm12"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm13"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm14"));
    cc->scratchRegisters().insert(regdict->findOrThrow("xmm15"));

    cc->scratchRegisters().insert(regdict->findOrThrow("st0")); // i.e., statically mm0
    cc->scratchRegisters().insert(regdict->findOrThrow("st1")); // mm1, etc. Since mm<i> could
    cc->scratchRegisters().insert(regdict->findOrThrow("st2")); // overlap with st(0) or st(1),
    cc->scratchRegisters().insert(regdict->findOrThrow("st3")); // two of these could also be
    cc->scratchRegisters().insert(regdict->findOrThrow("st4")); // return values.
    cc->scratchRegisters().insert(regdict->findOrThrow("st5"));
    cc->scratchRegisters().insert(regdict->findOrThrow("st6"));
    cc->scratchRegisters().insert(regdict->findOrThrow("st7"));

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());
    return cc;
}

} // namespace
} // namespace
} // namespace

#endif
