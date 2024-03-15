#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelI386::IntelI386()
    : X86("intel-i386", 4) {}

IntelI386::~IntelI386() {}

IntelI386::Ptr
IntelI386::instance() {
    return Ptr(new IntelI386);
}

RegisterDictionary::Ptr
IntelI386::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-80286").orThrow()->registerDictionary());

        // Additional 32-bit registers
        regs->insert("eax", x86_regclass_gpr, x86_gpr_ax, 0, 32);
        regs->insert("ebx", x86_regclass_gpr, x86_gpr_bx, 0, 32);
        regs->insert("ecx", x86_regclass_gpr, x86_gpr_cx, 0, 32);
        regs->insert("edx", x86_regclass_gpr, x86_gpr_dx, 0, 32);
        regs->insert("esp", x86_regclass_gpr, x86_gpr_sp, 0, 32);
        regs->insert("ebp", x86_regclass_gpr, x86_gpr_bp, 0, 32);
        regs->insert("eip", x86_regclass_ip, 0, 0, 32);
        regs->insert("esi", x86_regclass_gpr, x86_gpr_si, 0, 32);
        regs->insert("edi", x86_regclass_gpr, x86_gpr_di, 0, 32);
        regs->insert("eflags", x86_regclass_flags, x86_flags_status, 0, 32);

        // Additional 16-bit segment registers
        regs->insert("fs", x86_regclass_segment, x86_segreg_fs, 0, 16);
        regs->insert("gs", x86_regclass_segment, x86_segreg_gs, 0, 16);

        // Additional flags
        regs->insert("rf", x86_regclass_flags, x86_flags_status, 16, 1); // resume system flag
        regs->insert("vm", x86_regclass_flags, x86_flags_status, 17, 1); // virtual 8086 mode flag

        // Additional flag bits that have no official names
        for (unsigned i=18; i<32; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);

        // Control registers
        regs->insert("cr0", x86_regclass_cr, 0, 0, 32);
        regs->insert("cr1", x86_regclass_cr, 1, 0, 32);
        regs->insert("cr2", x86_regclass_cr, 2, 0, 32);
        regs->insert("cr3", x86_regclass_cr, 3, 0, 32);
        regs->insert("cr4", x86_regclass_cr, 4, 0, 32);

        // Debug registers
        regs->insert("dr0", x86_regclass_dr, 0, 0, 32);
        regs->insert("dr1", x86_regclass_dr, 1, 0, 32);
        regs->insert("dr2", x86_regclass_dr, 2, 0, 32);
        regs->insert("dr3", x86_regclass_dr, 3, 0, 32); // dr4 and dr5 are reserved
        regs->insert("dr6", x86_regclass_dr, 6, 0, 32);
        regs->insert("dr7", x86_regclass_dr, 7, 0, 32);

        // The 387 contains eight floating-point registers that have no names (we call them "st0" through "st7"), and defines
        // expressions of the form "st(n)" to refer to the current nth register from the top of a circular stack.  These
        // expressions are implemented usng SgAsmIndexedRegisterExpression IR nodes, which have a base register which is
        // "st0", a stride which increments the minor number, an offset which is the current top-of-stack value, an index
        // which is the value "n" in the expression "st(n)", and a modulus of eight.  The current top-of-stack value is held in
        // the three-bit register "fpstatus_top", which normally has a concrete value.
        regs->insert("st0",     x86_regclass_st, x86_st_0,   0, 80);
        regs->insert("st1",     x86_regclass_st, x86_st_1,   0, 80);
        regs->insert("st2",     x86_regclass_st, x86_st_2,   0, 80);
        regs->insert("st3",     x86_regclass_st, x86_st_3,   0, 80);
        regs->insert("st4",     x86_regclass_st, x86_st_4,   0, 80);
        regs->insert("st5",     x86_regclass_st, x86_st_5,   0, 80);
        regs->insert("st6",     x86_regclass_st, x86_st_6,   0, 80);
        regs->insert("st7",     x86_regclass_st, x86_st_7,   0, 80);

        // Floating-point tag registers, two bits per ST register.
        regs->insert("fptag",     x86_regclass_flags, x86_flags_fptag,  0, 16); // all tags
        regs->insert("fptag_st0", x86_regclass_flags, x86_flags_fptag,  0,  2); // tag for st0
        regs->insert("fptag_st1", x86_regclass_flags, x86_flags_fptag,  2,  2); // tag for st1
        regs->insert("fptag_st2", x86_regclass_flags, x86_flags_fptag,  4,  2); // tag for st2
        regs->insert("fptag_st3", x86_regclass_flags, x86_flags_fptag,  6,  2); // tag for st3
        regs->insert("fptag_st4", x86_regclass_flags, x86_flags_fptag,  8,  2); // tag for st4
        regs->insert("fptag_st5", x86_regclass_flags, x86_flags_fptag, 10,  2); // tag for st5
        regs->insert("fptag_st6", x86_regclass_flags, x86_flags_fptag, 12,  2); // tag for st6
        regs->insert("fptag_st7", x86_regclass_flags, x86_flags_fptag, 14,  2); // tag for st7

        // Floating-point status register
        regs->insert("fpstatus",     x86_regclass_flags, x86_flags_fpstatus,  0, 16);
        regs->insert("fpstatus_ie",  x86_regclass_flags, x86_flags_fpstatus,  0,  1); // invalid operation
        regs->insert("fpstatus_de",  x86_regclass_flags, x86_flags_fpstatus,  1,  1); // denormalized operand
        regs->insert("fpstatus_ze",  x86_regclass_flags, x86_flags_fpstatus,  2,  1); // zero divide
        regs->insert("fpstatus_oe",  x86_regclass_flags, x86_flags_fpstatus,  3,  1); // overflow
        regs->insert("fpstatus_ue",  x86_regclass_flags, x86_flags_fpstatus,  4,  1); // underflow
        regs->insert("fpstatus_pe",  x86_regclass_flags, x86_flags_fpstatus,  5,  1); // precision
        regs->insert("fpstatus_ir",  x86_regclass_flags, x86_flags_fpstatus,  7,  1); // interrupt request
        regs->insert("fpstatus_c4",  x86_regclass_flags, x86_flags_fpstatus,  8,  1); // condition code
        regs->insert("fpstatus_c1",  x86_regclass_flags, x86_flags_fpstatus,  9,  1); // condition code
        regs->insert("fpstatus_c2",  x86_regclass_flags, x86_flags_fpstatus, 10,  1); // condition code
        regs->insert("fpstatus_top", x86_regclass_flags, x86_flags_fpstatus, 11,  3); // top of stack
        regs->insert("fpstatus_c3",  x86_regclass_flags, x86_flags_fpstatus, 14,  1); // condition code
        regs->insert("fpstatus_b",   x86_regclass_flags, x86_flags_fpstatus, 15,  1); // busy

        // Floating-point control register
        regs->insert("fpctl",    x86_regclass_flags, x86_flags_fpctl,  0, 16);
        regs->insert("fpctl_im", x86_regclass_flags, x86_flags_fpctl,  0,  1); // invalid operation
        regs->insert("fpctl_dm", x86_regclass_flags, x86_flags_fpctl,  1,  1); // denormalized operand
        regs->insert("fpctl_zm", x86_regclass_flags, x86_flags_fpctl,  2,  1); // zero divide
        regs->insert("fpctl_om", x86_regclass_flags, x86_flags_fpctl,  3,  1); // overflow
        regs->insert("fpctl_um", x86_regclass_flags, x86_flags_fpctl,  4,  1); // underflow
        regs->insert("fpctl_pm", x86_regclass_flags, x86_flags_fpctl,  5,  1); // precision
        regs->insert("fpctl_m",  x86_regclass_flags, x86_flags_fpctl,  7,  1); // interrupt mask
        regs->insert("fpctl_pc", x86_regclass_flags, x86_flags_fpctl,  8,  2); // precision control
        regs->insert("fpctl_rc", x86_regclass_flags, x86_flags_fpctl, 10,  2); // rounding control
        regs->insert("fpctl_ic", x86_regclass_flags, x86_flags_fpctl, 12,  1); // infinity control

        // Special registers
        regs->instructionPointerRegister("eip");
        regs->stackPointerRegister("esp");
        regs->stackFrameRegister("ebp");
        regs->stackSegmentRegister("ss");

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelI386::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

} // namespace
} // namespace
} // namespace

#endif
