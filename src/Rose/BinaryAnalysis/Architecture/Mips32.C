#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Mips32.h>

#include <Rose/BinaryAnalysis/Disassembler/Mips.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesMips.h>
#include <Rose/BinaryAnalysis/Unparser/Mips.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Mips32::Mips32(ByteOrder::Endianness byteOrder)
    : Base(ByteOrder::ORDER_MSB == byteOrder ? "mips32-be" : (ByteOrder::ORDER_LSB == byteOrder ? "mips32-el" : "mips32"),
           4, byteOrder) {}

Mips32::~Mips32() {}

Mips32::Ptr
Mips32::instance(ByteOrder::Endianness byteOrder) {
    return Ptr(new Mips32(byteOrder));
}

RegisterDictionary::Ptr
Mips32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());

        // 32 general purpose registers and hardware registers
        for (size_t i=0; i<32; ++i) {
            regs->insert("r"+StringUtility::numberToString(i), mips_regclass_gpr, i, 0, 32);
            regs->insert("hw"+StringUtility::numberToString(i), mips_regclass_hw, i, 0, 32);
        }

        // Alternate names for the general purpose registers
        regs->insert("zero", mips_regclass_gpr, 0,  0, 32);                     // always equal to zero
        regs->insert("at",   mips_regclass_gpr, 1,  0, 32);                     // assembler temporary
        regs->insert("v0",   mips_regclass_gpr, 2,  0, 32);                     // return value from a function call
        regs->insert("v1",   mips_regclass_gpr, 3,  0, 32);
        regs->insert("a0",   mips_regclass_gpr, 4,  0, 32);                     // first four function arguments
        regs->insert("a1",   mips_regclass_gpr, 5,  0, 32);
        regs->insert("a2",   mips_regclass_gpr, 6,  0, 32);
        regs->insert("a3",   mips_regclass_gpr, 7,  0, 32);
        regs->insert("t0",   mips_regclass_gpr, 8,  0, 32);                     // temporary variables; need not be preserved
        regs->insert("t1",   mips_regclass_gpr, 9,  0, 32);
        regs->insert("t2",   mips_regclass_gpr, 10, 0, 32);
        regs->insert("t3",   mips_regclass_gpr, 11, 0, 32);
        regs->insert("t4",   mips_regclass_gpr, 12, 0, 32);
        regs->insert("t5",   mips_regclass_gpr, 13, 0, 32);
        regs->insert("t6",   mips_regclass_gpr, 14, 0, 32);
        regs->insert("t7",   mips_regclass_gpr, 15, 0, 32);
        regs->insert("s0",   mips_regclass_gpr, 16, 0, 32);                     // temporary variables; must be preserved
        regs->insert("s1",   mips_regclass_gpr, 17, 0, 32);
        regs->insert("s2",   mips_regclass_gpr, 18, 0, 32);
        regs->insert("s3",   mips_regclass_gpr, 19, 0, 32);
        regs->insert("s4",   mips_regclass_gpr, 20, 0, 32);
        regs->insert("s5",   mips_regclass_gpr, 21, 0, 32);
        regs->insert("s6",   mips_regclass_gpr, 22, 0, 32);
        regs->insert("s7",   mips_regclass_gpr, 23, 0, 32);
        regs->insert("t8",   mips_regclass_gpr, 24, 0, 32);                     // two more temporaries; need not be preserved
        regs->insert("t9",   mips_regclass_gpr, 25, 0, 32);
        regs->insert("k0",   mips_regclass_gpr, 26, 0, 32);                     // kernel use; may change unexpectedly
        regs->insert("k1",   mips_regclass_gpr, 27, 0, 32);
        regs->insert("gp",   mips_regclass_gpr, 28, 0, 32);                     // global pointer
        regs->insert("sp",   mips_regclass_gpr, 29, 0, 32);                     // stack pointer
        regs->insert("s8",   mips_regclass_gpr, 30, 0, 32);                     // temp; must be preserved (or "fp")
        regs->insert("fp",   mips_regclass_gpr, 30, 0, 32);                     // stack frame pointer (or "s8")
        regs->insert("ra",   mips_regclass_gpr, 31, 0, 32);                     // return address (link register)

        // Special purpose registers
        regs->insert("hi", mips_regclass_spr, mips_spr_hi, 0, 32);
        regs->insert("lo", mips_regclass_spr, mips_spr_lo, 0, 32);
        regs->insert("pc", mips_regclass_spr, mips_spr_pc, 0, 32);              // program counter

        // 32 floating point registers
        for (size_t i=0; i<32; ++i)
            regs->insert("f"+StringUtility::numberToString(i), mips_regclass_fpr, i, 0, 32);

        // Five FPU control registers are used to identify and control the FPU. The FCCR, FEXR, and FENR are portions
        // (not necessarily contiguous) of the FCSR extended to 32 bits, and therefore all share a major number.
        regs->insert("fir", mips_regclass_spr, mips_spr_fir, 0, 32);            // FP implementation and revision
        regs->insert("fcsr", mips_regclass_fcsr, mips_fcsr_all, 0, 32);         // the entire FCSR register
        regs->insert("fccr", mips_regclass_fcsr, mips_fcsr_fccr, 0, 32);        // condition codes portion of FCSR
        regs->insert("fexr", mips_regclass_fcsr, mips_fcsr_fexr, 0, 32);        // FP exceptions
        regs->insert("fenr", mips_regclass_fcsr, mips_fcsr_fenr, 0, 32);        // FP enables

        // parts of the FIR (only those defined for MIPS32 release 1)
        regs->insert("fir.d", mips_regclass_spr, mips_spr_fir, 17, 1);          // is double-precision implemented?
        regs->insert("fir.s", mips_regclass_spr, mips_spr_fir, 16, 1);          // is single-precision implemented?
        regs->insert("fir.processorid", mips_regclass_spr, mips_spr_fir, 8, 8); // identifies the FP processor
        regs->insert("fir.revision", mips_regclass_spr, mips_spr_fir, 0, 8);    // FP unit revision number

        // Additional registers for coprocessor 0 are not part of this dictionary. They use major number mips_regclass_cp0gpr.

        // Additional implementation-specific coprocessor 2 registers are not part of the dictionary. Coprocessor 2 may have up
        // to 32 general purpose registers and up to 32 control registers.  They use the major numbers mips_regclass_cp2gpr and
        // mips_regclass_cp2spr.

        // Special registers
        regs->instructionPointerRegister("pc");
        regs->stackPointerRegister("sp");
        regs->stackFrameRegister("fp");
        regs->callReturnRegister("ra");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Mips32::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_MIPS_Family;
}

Disassembler::Base::Ptr
Mips32::newInstructionDecoder() const {
    return Disassembler::Mips::instance(shared_from_this());
}

Unparser::Base::Ptr
Mips32::newUnparser() const {
    return Unparser::Mips::instance(shared_from_this());
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Mips32::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesMips::MatchRetAddiu::instance());
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
