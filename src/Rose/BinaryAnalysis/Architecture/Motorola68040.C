#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Unparser/M68k.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Motorola68040::Motorola68040()
    : Motorola("motorola-68040") {}

Motorola68040::~Motorola68040() {}

Motorola68040::Ptr
Motorola68040::instance() {
    return Ptr(new Motorola68040);
}

RegisterDictionary::Ptr
Motorola68040::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());

        // 32-bit integer data and address registers. When the low-order 16-bits of a data or address register is accessed by a
        // word instruction, or the low-order 8-bits of a data register is accessed by a byte instruction, the size of the
        // access is implied by the suffix of the instruction (e.g., "MOV.B" vs "MOV.W", vs. MOV.L") and the usual m68k
        // assembly listings to not have special names for the register parts.  ROSE on the other hand gives names to each
        // register part.
        for (size_t i=0; i<8; ++i) {
            std::string regnum = StringUtility::numberToString(i);
            regs->insert("d"+regnum,       m68k_regclass_data, i,  0, 32);
            regs->insert("d"+regnum+".w0", m68k_regclass_data, i,  0, 16);
            regs->insert("d"+regnum+".w1", m68k_regclass_data, i, 16, 16);
            regs->insert("d"+regnum+".b0", m68k_regclass_data, i,  0,  8);
            regs->insert("d"+regnum+".b1", m68k_regclass_data, i,  8,  8);
            regs->insert("d"+regnum+".b2", m68k_regclass_data, i, 12,  8);
            regs->insert("d"+regnum+".b3", m68k_regclass_data, i, 16,  8);
            regs->insert("a"+regnum,       m68k_regclass_addr, i,  0, 32);
            regs->insert("a"+regnum+".w0", m68k_regclass_addr, i,  0, 16);
            regs->insert("a"+regnum+".w1", m68k_regclass_addr, i, 16, 16);
        }

        // Special-purpose registers
        regs->insert("pc",    m68k_regclass_spr, m68k_spr_pc, 0,  32);          // program counter
        regs->insert("ccr",   m68k_regclass_spr, m68k_spr_sr, 0,  8);           // condition code register
        regs->insert("ccr_c", m68k_regclass_spr, m68k_spr_sr, 0,  1);           // condition code carry bit
        regs->insert("ccr_v", m68k_regclass_spr, m68k_spr_sr, 1,  1);           // condition code overflow
        regs->insert("ccr_z", m68k_regclass_spr, m68k_spr_sr, 2,  1);           // condition code zero bit
        regs->insert("ccr_n", m68k_regclass_spr, m68k_spr_sr, 3,  1);           // condition code negative
        regs->insert("ccr_x", m68k_regclass_spr, m68k_spr_sr, 4,  1);           // condition code extend
        regs->insert("ps",    m68k_regclass_spr, m68k_spr_sr, 0,  16);          // GDB's name for the SR register
        regs->insert("sr",    m68k_regclass_spr, m68k_spr_sr, 0,  16);          // status register
        regs->insert("sr_i",  m68k_regclass_spr, m68k_spr_sr, 8,  3);           // interrupt priority mask
        regs->insert("sr_s",  m68k_regclass_spr, m68k_spr_sr, 13, 1);           // status register user mode bit
        regs->insert("sr_t",  m68k_regclass_spr, m68k_spr_sr, 14, 2);           // status register trace mode bits

        // Floating point data registers
        // These registers hold 96-bit extended-precision real format ("X") values. However, since the X format has
        // 16 reserved zero bits at positions 64-79, inclusive, the value can be stored in 80 bits.  Therefore, the
        // floating point data registers are only 80-bits wide.
        for (size_t i=0; i<8; ++i)
            regs->insert("fp"+StringUtility::numberToString(i),      m68k_regclass_fpr,  i, 0,  80);

        // Floating point control registers
        regs->insert("fpcr",       m68k_regclass_spr, m68k_spr_fpcr,  0, 32);   // floating-point control register
        regs->insert("fpcr_mctl",  m68k_regclass_spr, m68k_spr_fpcr,  0,  8);   // mode control
        regs->insert("fpcr_xen",   m68k_regclass_spr, m68k_spr_fpcr,  8,  8);   // exception enable
        regs->insert("fpcr_rnd",   m68k_regclass_spr, m68k_spr_fpcr,  4,  2);   // rounding mode
        regs->insert("fpcr_prec",  m68k_regclass_spr, m68k_spr_fpcr,  6,  2);   // rounding precision
        regs->insert("fpcr_inex1", m68k_regclass_spr, m68k_spr_fpcr,  8,  1);   // inexact decimal input
        regs->insert("fpcr_inex2", m68k_regclass_spr, m68k_spr_fpcr,  9,  1);   // inexact operation
        regs->insert("fpcr_dz",    m68k_regclass_spr, m68k_spr_fpcr, 10,  1);   // divide by zero
        regs->insert("fpcr_unfl",  m68k_regclass_spr, m68k_spr_fpcr, 11,  1);   // underflow
        regs->insert("fpcr_ovfl",  m68k_regclass_spr, m68k_spr_fpcr, 12,  1);   // overflow
        regs->insert("fpcr_operr", m68k_regclass_spr, m68k_spr_fpcr, 13,  1);   // operand error
        regs->insert("fpcr_snan",  m68k_regclass_spr, m68k_spr_fpcr, 14,  1);   // signaling not-a-number
        regs->insert("fpcr_bsun",  m68k_regclass_spr, m68k_spr_fpcr, 15,  1);   // branch/set on unordered

        // Floating point status registers
        regs->insert("fpstatus",   m68k_regclass_spr, m68k_spr_fpsr,  0, 32);   // GDB's name for "fpsr"
        regs->insert("fpsr",       m68k_regclass_spr, m68k_spr_fpsr,  0, 32);   // floating-point status register
        regs->insert("fpsr_aexc",  m68k_regclass_spr, m68k_spr_fpsr,  0,  8);   // accrued exception status
        regs->insert("aexc_inex",  m68k_regclass_spr, m68k_spr_fpsr,  3,  1);   // inexact
        regs->insert("aexc_dz",    m68k_regclass_spr, m68k_spr_fpsr,  4,  1);   // divide by zero
        regs->insert("aexc_unfl",  m68k_regclass_spr, m68k_spr_fpsr,  5,  1);   // underflow
        regs->insert("aexc_ovfl",  m68k_regclass_spr, m68k_spr_fpsr,  6,  1);   // overflow
        regs->insert("aexc_iop",   m68k_regclass_spr, m68k_spr_fpsr,  7,  1);   // invalid operation
        regs->insert("fpsr_exc",   m68k_regclass_spr, m68k_spr_fpsr,  8,  8);   // exception status
        regs->insert("exc_inex1",  m68k_regclass_spr, m68k_spr_fpsr,  8,  1);   // inexact decimal input (input is denormalized)
        regs->insert("exc_inex2",  m68k_regclass_spr, m68k_spr_fpsr,  9,  1);   // inexact operation (inexact result)
        regs->insert("exc_dz",     m68k_regclass_spr, m68k_spr_fpsr, 10,  1);   // divide by zero
        regs->insert("exc_unfl",   m68k_regclass_spr, m68k_spr_fpsr, 11,  1);   // underflow
        regs->insert("exc_ovfl",   m68k_regclass_spr, m68k_spr_fpsr, 12,  1);   // overflow
        regs->insert("exc_operr",  m68k_regclass_spr, m68k_spr_fpsr, 13,  1);   // operand error
        regs->insert("exc_snan",   m68k_regclass_spr, m68k_spr_fpsr, 14,  1);   // signaling not-a-number
        regs->insert("exc_bsun",   m68k_regclass_spr, m68k_spr_fpsr, 15,  1);   // branch/set on unordered
        regs->insert("fpsr_quot",  m68k_regclass_spr, m68k_spr_fpsr, 16,  8);   // quotient
        regs->insert("fpcc",       m68k_regclass_spr, m68k_spr_fpsr, 24,  8);   // floating-point conditon code
        regs->insert("fpcc_nan",   m68k_regclass_spr, m68k_spr_fpsr, 24,  1);   // not-a-number or unordered
        regs->insert("fpcc_i",     m68k_regclass_spr, m68k_spr_fpsr, 25,  1);   // infinity
        regs->insert("fpcc_z",     m68k_regclass_spr, m68k_spr_fpsr, 25,  1);   // zero
        regs->insert("fpcc_n",     m68k_regclass_spr, m68k_spr_fpsr, 26,  1);   // negative

        // Other floating point registers
        regs->insert("fpaddr",     m68k_regclass_spr, m68k_spr_fpiar, 0, 32);   // GDB's name for "fpiar"
        regs->insert("fpiar",      m68k_regclass_spr, m68k_spr_fpiar, 0, 32);   // floating-point instruction address reg

        // Supervisor registers (SR register is listed above since its CCR bits are available in user mode)
        regs->insert("ssp",      m68k_regclass_sup, m68k_sup_ssp,       0, 32); // supervisor A7 stack pointer
        regs->insert("vbr",      m68k_regclass_sup, m68k_sup_vbr,       0, 32); // vector base register

        // Alternate names
        regs->insert("fp", m68k_regclass_addr, 6, 0, 32);                       // a6 is conventionally the stack frame pointer
        regs->insert("sp", m68k_regclass_addr, 7, 0, 32);                       // a7 is conventionally the stack pointer

        // Special registers
        regs->instructionPointerRegister("pc");
        regs->stackPointerRegister("a7");
        regs->stackFrameRegister("a6");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Motorola68040::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

Disassembler::Base::Ptr
Motorola68040::newInstructionDecoder() const {
    return Disassembler::M68k::instance(shared_from_this(), m68k_68040);
}

} // namespace
} // namespace
} // namespace

#endif
