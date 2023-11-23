#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>
#include <Rose/BinaryAnalysis/InstructionEnumsM68k.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>
#include <Rose/BinaryAnalysis/InstructionEnumsPowerpc.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/StringUtility/Escape.h>

#include <Sawyer/Assert.h>

namespace Rose {
namespace BinaryAnalysis {

RegisterDictionary::RegisterDictionary() {}

RegisterDictionary::RegisterDictionary(const std::string &name)
    : name_(name) {}

RegisterDictionary::RegisterDictionary(const RegisterDictionary &other)
    : SharedObject(other) {
    *this = other;
}

RegisterDictionary::Ptr
RegisterDictionary::instance(const std::string &name) {
    return Ptr(new RegisterDictionary(name));
}

RegisterDictionary::Ptr
RegisterDictionary::instanceNull() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs =RegisterDictionary::instance("null");
        regs->insert("pc", 0, 0, 0, 8);                 // program counter
        regs->insert("sp", 0, 1, 0, 8);                 // stack pointer
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceI8086() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("i8086");
        /*  16-bit general purpose registers. Each has three names depending on which bytes are reference. */
        regs->insert("al", x86_regclass_gpr, x86_gpr_ax, 0, 8);
        regs->insert("ah", x86_regclass_gpr, x86_gpr_ax, 8, 8);
        regs->insert("ax", x86_regclass_gpr, x86_gpr_ax, 0, 16);

        regs->insert("bl", x86_regclass_gpr, x86_gpr_bx, 0, 8);
        regs->insert("bh", x86_regclass_gpr, x86_gpr_bx, 8, 8);
        regs->insert("bx", x86_regclass_gpr, x86_gpr_bx, 0, 16);

        regs->insert("cl", x86_regclass_gpr, x86_gpr_cx, 0, 8);
        regs->insert("ch", x86_regclass_gpr, x86_gpr_cx, 8, 8);
        regs->insert("cx", x86_regclass_gpr, x86_gpr_cx, 0, 16);

        regs->insert("dl", x86_regclass_gpr, x86_gpr_dx, 0, 8);
        regs->insert("dh", x86_regclass_gpr, x86_gpr_dx, 8, 8);
        regs->insert("dx", x86_regclass_gpr, x86_gpr_dx, 0, 16);

        /*  16-bit segment registers */
        regs->insert("cs", x86_regclass_segment, x86_segreg_cs, 0, 16);
        regs->insert("ds", x86_regclass_segment, x86_segreg_ds, 0, 16);
        regs->insert("ss", x86_regclass_segment, x86_segreg_ss, 0, 16);
        regs->insert("es", x86_regclass_segment, x86_segreg_es, 0, 16);

        /* 16-bit pointer registers */
        regs->insert("sp", x86_regclass_gpr, x86_gpr_sp, 0, 16);        /* stack pointer */
        regs->insert("spl", x86_regclass_gpr, x86_gpr_sp, 0, 8);

        regs->insert("bp", x86_regclass_gpr, x86_gpr_bp, 0, 16);        /* base pointer */
        regs->insert("bpl", x86_regclass_gpr, x86_gpr_bp, 0, 8);

        regs->insert("ip", x86_regclass_ip, 0, 0, 16);                  /* instruction pointer */
        regs->insert("ipl", x86_regclass_ip, 0, 0, 8);

        /* Array indexing registers */
        regs->insert("si", x86_regclass_gpr, x86_gpr_si, 0, 16);
        regs->insert("sil", x86_regclass_gpr, x86_gpr_si, 0, 8);

        regs->insert("di", x86_regclass_gpr, x86_gpr_di, 0, 16);
        regs->insert("dil", x86_regclass_gpr, x86_gpr_di, 0, 8);

        /* Flags with official names. */
        regs->insert("flags", x86_regclass_flags, x86_flags_status,  0, 16); /* all flags */
        regs->insert("cf",    x86_regclass_flags, x86_flags_status,  0,  1); /* carry status flag */
        regs->insert("pf",    x86_regclass_flags, x86_flags_status,  2,  1); /* parity status flag */
        regs->insert("af",    x86_regclass_flags, x86_flags_status,  4,  1); /* adjust status flag */
        regs->insert("zf",    x86_regclass_flags, x86_flags_status,  6,  1); /* zero status flag */
        regs->insert("sf",    x86_regclass_flags, x86_flags_status,  7,  1); /* sign status flag */
        regs->insert("tf",    x86_regclass_flags, x86_flags_status,  8,  1); /* trap system flag */
        regs->insert("if",    x86_regclass_flags, x86_flags_status,  9,  1); /* interrupt enable system flag */
        regs->insert("df",    x86_regclass_flags, x86_flags_status, 10,  1); /* direction control flag */
        regs->insert("of",    x86_regclass_flags, x86_flags_status, 11,  1); /* overflow status flag */
        regs->insert("nt",    x86_regclass_flags, x86_flags_status, 14,  1); /* nested task system flag */

        /* Flags without names */
        regs->insert("f1",    x86_regclass_flags, x86_flags_status,  1,  1);
        regs->insert("f3",    x86_regclass_flags, x86_flags_status,  3,  1);
        regs->insert("f5",    x86_regclass_flags, x86_flags_status,  5,  1);
        regs->insert("f12",   x86_regclass_flags, x86_flags_status, 12,  1);
        regs->insert("f13",   x86_regclass_flags, x86_flags_status, 13,  1);
        regs->insert("f15",   x86_regclass_flags, x86_flags_status, 15,  1);

        // Special registers
        regs->instructionPointerRegister("ip");
        regs->stackPointerRegister("sp");
        regs->stackFrameRegister("bp");
        regs->stackSegmentRegister("ss");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceI8088() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("i8088");
        regs->insert(instanceI8086());
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceI286() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("i286");
        regs->insert(instanceI8086());
        regs->insert("iopl", x86_regclass_flags, x86_flags_status, 12, 2); /*  I/O privilege level flag */
        regs->insert("nt",   x86_regclass_flags, x86_flags_status, 14, 1); /*  nested task system flag */
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceI386() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("i386");
        regs->insert(instanceI286());

        /* Additional 32-bit registers */
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

        /* Additional 16-bit segment registers */
        regs->insert("fs", x86_regclass_segment, x86_segreg_fs, 0, 16);
        regs->insert("gs", x86_regclass_segment, x86_segreg_gs, 0, 16);

        /* Additional flags */
        regs->insert("rf", x86_regclass_flags, x86_flags_status, 16, 1); /* resume system flag */
        regs->insert("vm", x86_regclass_flags, x86_flags_status, 17, 1); /* virtual 8086 mode flag */

        /* Additional flag bits that have no official names */
        for (unsigned i=18; i<32; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);

        /* Control registers */
        regs->insert("cr0", x86_regclass_cr, 0, 0, 32);
        regs->insert("cr1", x86_regclass_cr, 1, 0, 32);
        regs->insert("cr2", x86_regclass_cr, 2, 0, 32);
        regs->insert("cr3", x86_regclass_cr, 3, 0, 32);
        regs->insert("cr4", x86_regclass_cr, 4, 0, 32);

        /* Debug registers */
        regs->insert("dr0", x86_regclass_dr, 0, 0, 32);
        regs->insert("dr1", x86_regclass_dr, 1, 0, 32);
        regs->insert("dr2", x86_regclass_dr, 2, 0, 32);
        regs->insert("dr3", x86_regclass_dr, 3, 0, 32);                 /* dr4 and dr5 are reserved */
        regs->insert("dr6", x86_regclass_dr, 6, 0, 32);
        regs->insert("dr7", x86_regclass_dr, 7, 0, 32);

        // Special registers
        regs->instructionPointerRegister("eip");
        regs->stackPointerRegister("esp");
        regs->stackFrameRegister("ebp");
        regs->stackSegmentRegister("ss");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceI386Math() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("i386 w/387");
        regs->insert(instanceI386());

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
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceI486() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("i486");
        regs->insert(instanceI386Math());
        regs->insert("ac", x86_regclass_flags, x86_flags_status, 18, 1); /* alignment check system flag */
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instancePentium() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("pentium");
        regs->insert(instanceI486());

        /* Additional flags */
        regs->insert("vif", x86_regclass_flags, x86_flags_status, 19, 1); /* virtual interrupt flag */
        regs->insert("vip", x86_regclass_flags, x86_flags_status, 20, 1); /* virt interrupt pending */
        regs->insert("id",  x86_regclass_flags, x86_flags_status, 21, 1); /* ident system flag */

        /* The MMi registers are aliases for the ST(i) registers but are absolute rather than relative to the top of the
         * stack. We're creating the static definitions, so MMi will point to the same storage as ST(i) for 0<=i<=7. Note that
         * a write to one of the 64-bit MMi registers causes the high-order 16 bits of the corresponding ST(j) register to be
         * set to all ones to indicate a NaN value. */
        regs->insert("mm0", x86_regclass_st, x86_st_0, 0, 64);
        regs->insert("mm1", x86_regclass_st, x86_st_1, 0, 64);
        regs->insert("mm2", x86_regclass_st, x86_st_2, 0, 64);
        regs->insert("mm3", x86_regclass_st, x86_st_3, 0, 64);
        regs->insert("mm4", x86_regclass_st, x86_st_4, 0, 64);
        regs->insert("mm5", x86_regclass_st, x86_st_5, 0, 64);
        regs->insert("mm6", x86_regclass_st, x86_st_6, 0, 64);
        regs->insert("mm7", x86_regclass_st, x86_st_7, 0, 64);
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instancePentiumiii() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("pentiumiii");
        regs->insert(instancePentium());
        regs->insert("xmm0", x86_regclass_xmm, 0, 0, 128);
        regs->insert("xmm1", x86_regclass_xmm, 1, 0, 128);
        regs->insert("xmm2", x86_regclass_xmm, 2, 0, 128);
        regs->insert("xmm3", x86_regclass_xmm, 3, 0, 128);
        regs->insert("xmm4", x86_regclass_xmm, 4, 0, 128);
        regs->insert("xmm5", x86_regclass_xmm, 5, 0, 128);
        regs->insert("xmm6", x86_regclass_xmm, 6, 0, 128);
        regs->insert("xmm7", x86_regclass_xmm, 7, 0, 128);

        /** SSE status and control register. */
        regs->insert("mxcsr",     x86_regclass_flags, x86_flags_mxcsr,  0, 32);
        regs->insert("mxcsr_ie",  x86_regclass_flags, x86_flags_mxcsr,  0,  1); // invalid operation flag
        regs->insert("mxcsr_de",  x86_regclass_flags, x86_flags_mxcsr,  1,  1); // denormal flag
        regs->insert("mxcsr_ze",  x86_regclass_flags, x86_flags_mxcsr,  2,  1); // divide by zero flag
        regs->insert("mxcsr_oe",  x86_regclass_flags, x86_flags_mxcsr,  3,  1); // overflow flag
        regs->insert("mxcsr_ue",  x86_regclass_flags, x86_flags_mxcsr,  4,  1); // underflow flag
        regs->insert("mxcsr_pe",  x86_regclass_flags, x86_flags_mxcsr,  5,  1); // precision flag
        regs->insert("mxcsr_daz", x86_regclass_flags, x86_flags_mxcsr,  6,  1); // denormals are zero
        regs->insert("mxcsr_im",  x86_regclass_flags, x86_flags_mxcsr,  7,  1); // invalid operation mask
        regs->insert("mxcsr_dm",  x86_regclass_flags, x86_flags_mxcsr,  8,  1); // denormal mask
        regs->insert("mxcsr_zm",  x86_regclass_flags, x86_flags_mxcsr,  9,  1); // divide by zero mask
        regs->insert("mxcsr_om",  x86_regclass_flags, x86_flags_mxcsr, 10,  1); // overflow mask
        regs->insert("mxcsr_um",  x86_regclass_flags, x86_flags_mxcsr, 11,  1); // underflow mask
        regs->insert("mxcsr_pm",  x86_regclass_flags, x86_flags_mxcsr, 12,  1); // precision mask
        regs->insert("mxcsr_r",   x86_regclass_flags, x86_flags_mxcsr, 13,  2); // rounding mode
        regs->insert("mxcsr_fz",  x86_regclass_flags, x86_flags_mxcsr, 15,  1); // flush to zero
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instancePentium4() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("pentium4");
        regs->insert(instancePentiumiii());
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceAmd64() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("amd64");
        regs->insert(instancePentium4());

        /* Additional 64-bit (and hi-end 32-bit) registers */
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
            /* New general purpose registers in various widths */
            std::string name = "r" + StringUtility::numberToString(i);
            regs->insert(name,     x86_regclass_gpr, i, 0, 64);
            regs->insert(name+"b", x86_regclass_gpr, i, 0,  8);
            regs->insert(name+"w", x86_regclass_gpr, i, 0, 16);
            regs->insert(name+"d", x86_regclass_gpr, i, 0, 32);

            /* New media XMM registers */
            regs->insert(std::string("xmm")+StringUtility::numberToString(i),
                         x86_regclass_xmm, i, 0, 128);
        }

        /* Additional flag bits with no official names */
        for (unsigned i=32; i<64; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);

        /* Control registers become 64 bits, and cr8 is added */
        regs->resize("cr0", 64);
        regs->resize("cr1", 64);
        regs->resize("cr2", 64);
        regs->resize("cr3", 64);
        regs->resize("cr4", 64);
        regs->insert("cr8", x86_regclass_cr, 8, 0, 64);

        /* Debug registers become 64 bits */
        regs->resize("dr0", 64);
        regs->resize("dr1", 64);
        regs->resize("dr2", 64);
        regs->resize("dr3", 64);                                /* dr4 and dr5 are reserved */
        regs->resize("dr6", 64);
        regs->resize("dr7", 64);

        // Special registers
        regs->instructionPointerRegister("rip");
        regs->stackPointerRegister("rsp");
        regs->stackFrameRegister("rbp");
        regs->stackSegmentRegister("ss");
    }
    return regs;
}

#ifdef ROSE_ENABLE_ASM_AARCH64
RegisterDictionary::Ptr
RegisterDictionary::instanceAarch64() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    static RegisterDictionary::Ptr regs;
    if (!regs) {
        // References:
        //   [1] "Arm Instruction Set Version 1.0 Reference Guide" copyright 2018 Arm Limited.
        //   [2] "ARM Cortex-A Series Version 1.0 Programmer's Guide for ARMv8-A" copyright 2015 ARM.
        regs = RegisterDictionary::instance("AArch64");

        // 31 64-bit general-purpose registers "x0" through "x30". The names "w0" through "w30" refer to the low-order 32 bits
        // of the corresponding "x" register in that read operations read only the low-order 32 bits and write operations write
        // to all 64 bits (always clearing the high-order 32 bits). [1, p. A3-72]
        //
        for (unsigned i = 0; i < 31; ++i) {
            regs->insert("x" + boost::lexical_cast<std::string>(i), aarch64_regclass_gpr, i, 0, 64);
            regs->insert("w" + boost::lexical_cast<std::string>(i), aarch64_regclass_gpr, i, 0, 32);
        }

        // Zero register. Also called "register 31" in the documentation ("sp", though distinct from the zero register, is also
        // called "register 31" in the documentation. [1, pp. A3-72, A3-76]
        regs->insert("xzr", aarch64_regclass_gpr, 31, 0, 64);
        regs->insert("wzr", aarch64_regclass_gpr, 31, 0, 32);

        // Another name for "x30" is "lr", the "procedure link register". [1. pp. A3-74, A3-76]
        regs->insert("lr", aarch64_regclass_gpr, 30, 0, 64);

        // Another name for "x29" is "fp", the frame pointer. [2, p 4-2]
        regs->insert("fp", aarch64_regclass_gpr, 29, 0, 64);

        // One program counter. Not a general purpose register. [1. p. A3-78]
        regs->insert("pc", aarch64_regclass_pc, 0, 0, 64);

        // Four stack pointer registers. [1. p. A3-72] These store the exception return address per exception level. The
        // "sp_el0" is an alias for the stack pointer "sp". [1. p. A3-75] The "sp" register is also referred to as "register 31"
        // in the documentation [1. p. A3-72] (along with "xzr" and "wzr", which is clearly a distinct register) although this
        // register is also claimed to be not general-purpose like all the other "x" and "w" registers. [1. p. A3-75].
        regs->insert("sp_el0", aarch64_regclass_sp, 0, 0, 64); // "sp_el0" is an alias for "sp" [1. p. A3-75]
        regs->insert("sp_el1", aarch64_regclass_sp, 1, 0, 64);
        regs->insert("sp_el2", aarch64_regclass_sp, 2, 0, 64);
        regs->insert("sp_el3", aarch64_regclass_sp, 3, 0, 64);
        regs->insert("sp",     aarch64_regclass_sp, 0, 0, 64);
        regs->insert("wsp",    aarch64_regclass_sp, 0, 0, 32); // [2, p. 4-3]

        // Three saved program status registers. [1. p. A3-72, A3-82]. These store the processor state fields per exception
        // level so they can be restored when the exception handler returns. The processor state registers have various fields
        // defined in [2, p. 4-5]. Unlike AArch32, there is no processor state register that's accessible directly from assembly
        // instructions, but ROSE still must define it because it does exist internally. ROSE gives it the minor number zero and
        // the base name "cpsr" (the same name as from AArch32).
        for (unsigned i = 0; i < 4; ++i) {
            std::string base = 0==i ? "cpsr" : "spsr_el" + boost::lexical_cast<std::string>(i);
            regs->insert(base, aarch64_regclass_system, aarch64_system_spsr+i, 0, 32);
            regs->insert(base+".n",  aarch64_regclass_system, aarch64_system_spsr+i, 31, 1);     // negative condition flag
            regs->insert(base+".z",  aarch64_regclass_system, aarch64_system_spsr+i, 30, 1);     // zero condition flag
            regs->insert(base+".c",  aarch64_regclass_system, aarch64_system_spsr+i, 29, 1);     // carry condition flag
            regs->insert(base+".v",  aarch64_regclass_system, aarch64_system_spsr+i, 28, 1);     // overflow condition flag
            regs->insert(base+".ss", aarch64_regclass_system, aarch64_system_spsr+i, 21, 1);     // software step bit
            regs->insert(base+".il", aarch64_regclass_system, aarch64_system_spsr+i, 20, 1);     // illegal excecution bit
            regs->insert(base+".d",  aarch64_regclass_system, aarch64_system_spsr+i, 9, 1);      // debug mask bit
            regs->insert(base+".a",  aarch64_regclass_system, aarch64_system_spsr+i, 8, 1);      // "SError" mask bit
            regs->insert(base+".i",  aarch64_regclass_system, aarch64_system_spsr+i, 7, 1);      // IRQ mask bit
            regs->insert(base+".f",  aarch64_regclass_system, aarch64_system_spsr+i, 6, 1);      // FIQ mask bit
            regs->insert(base+".m",  aarch64_regclass_system, aarch64_system_spsr+i, 0, 4);      // source of exception
        }

        // System registers [2, p. 4-7]
        for (unsigned i = 0; i < 4; ++i) {
            std::string n = boost::lexical_cast<std::string>(i);
            // FPCR?
            // FPSR?
            if (i == 0) {
                regs->insert("cntfrq_el"+n,    aarch64_regclass_system, aarch64_system_cntfrq+i,    0, 64); // ctr-timer frequency reg
                regs->insert("cntpct_el"+n,    aarch64_regclass_system, aarch64_system_cntpct+i,    0, 64); // ctr-timer phys count reg
                regs->insert("cntp_cval_el"+n, aarch64_regclass_system, aarch64_system_cntp_cval+i, 0, 64); // ctr-timer phys timer cmp
                regs->insert("cntp_ctl_el"+n,  aarch64_regclass_system, aarch64_system_cntp_ctl+i,  0, 64); // ctr-timer phys control reg
                regs->insert("ctr_el"+n,       aarch64_regclass_system, aarch64_system_ctr+i,       0, 64); // cache type register
                regs->insert("dczid_el"+n,     aarch64_regclass_system, aarch64_system_dczid+i,     0, 64); // data cache zero ID reg
                regs->insert("tpidrr0_el"+n,   aarch64_regclass_system, aarch64_system_tpidrr0+i,   0, 64); // user read-only thread ID reg
            }
            if (i == 1) {
                regs->insert("ccsidr_el"+n,  aarch64_regclass_system, aarch64_system_ccsidr+i,  0, 64); // current cache size ID register
                regs->insert("cntkctl_el"+n, aarch64_regclass_system, aarch64_system_cntkctl+i, 0, 64); // counter-timer kernel control reg
                regs->insert("cpacr_el"+n,   aarch64_regclass_system, aarch64_system_cpacr+i,   0, 64); // coprocessor access control reg
                regs->insert("csselr_el"+n,  aarch64_regclass_system, aarch64_system_csselr+i,  0, 64); // cache size selection register
                regs->insert("midr_el"+n,    aarch64_regclass_system, aarch64_system_midr+i,    0, 64); // main ID register
                regs->insert("mpidr_el"+n,   aarch64_regclass_system, aarch64_system_mpidr+i,   0, 64); // multiprocessor affinity reg
                regs->insert("ttbr1_el"+n,   aarch64_regclass_system, aarch64_system_ttbr1+i,   0, 64); // translation table base reg 1
            }
            if (i == 2) {
                regs->insert("hcr_el"+n,   aarch64_regclass_system, aarch64_system_hcr+i,   0, 64); // hypervisor configuration register
                regs->insert("vtcr_el"+n,  aarch64_regclass_system, aarch64_system_vtcr+i,  0, 64); // virtualization translation ctr reg
                regs->insert("vttbr_el"+n, aarch64_regclass_system, aarch64_system_vttbr+i, 0, 64); // virt translation table base reg
            }
            if (i == 3) {
                regs->insert("scr_el"+n,   aarch64_regclass_system, aarch64_system_scr+i,   0, 64); // secure configuration register

            }
            if (true) {
                regs->insert("sctlr_el"+n, aarch64_regclass_system, aarch64_system_sctlr+i, 0, 32); // system control register [2, p. 4-10]
                {
                    // Not all bits are available above EL1, but ROSE defines them across the board anyway. Also the
                    // documentation is confusing: it says in one place [2, p. 4-9] that this register is defined for EL0
                    // through EL3, but then in another place [2, p. 4-10] that the bit fields are only defined for EL1 through
                    // EL3.
                    regs->insert("sctlr_el"+n+".uci",     aarch64_regclass_system, aarch64_system_sctlr+i, 26, 1); // enable EL0 access
                    regs->insert("sctlr_el"+n+".ee",      aarch64_regclass_system, aarch64_system_sctlr+i, 25, 1); // exception endianness
                    regs->insert("sctlr_el"+n+".eoe",     aarch64_regclass_system, aarch64_system_sctlr+i, 24, 1); // endianness of explicit data accesses at EL0
                    regs->insert("sctlr_el"+n+".wxn",     aarch64_regclass_system, aarch64_system_sctlr+i, 19, 1); // write permission implies execute never
                    regs->insert("sctlr_el"+n+".ntwe",    aarch64_regclass_system, aarch64_system_sctlr+i, 18, 1); // not trap WFE
                    regs->insert("sctlr_el"+n+".ntwi",    aarch64_regclass_system, aarch64_system_sctlr+i, 16, 1); // not trap WFI
                    regs->insert("sctlr_el"+n+".uct",     aarch64_regclass_system, aarch64_system_sctlr+i, 15, 1); // enable EL0 access to CTR_EL0 register
                    regs->insert("sctlr_el"+n+".dze",     aarch64_regclass_system, aarch64_system_sctlr+i, 14, 1); // access to DC ZVA instruction at EL0
                    regs->insert("sctlr_el"+n+".uma",     aarch64_regclass_system, aarch64_system_sctlr+i,  9, 1); // user mask access
                    regs->insert("sctlr_el"+n+".sed",     aarch64_regclass_system, aarch64_system_sctlr+i,  8, 1); // SETEND disable
                    regs->insert("sctlr_el"+n+".itd",     aarch64_regclass_system, aarch64_system_sctlr+i,  7, 1); // IT disable
                    regs->insert("sctlr_el"+n+".cp15ben", aarch64_regclass_system, aarch64_system_sctlr+i,  5, 1); // CP15 barrier enable
                    regs->insert("sctlr_el"+n+".sa0",     aarch64_regclass_system, aarch64_system_sctlr+i,  4, 1); // stack alignment check enable for EL0
                    regs->insert("sctlr_el"+n+".sa",      aarch64_regclass_system, aarch64_system_sctlr+i,  3, 1); // stack alignment check enable
                    regs->insert("sctlr_el"+n+".c",       aarch64_regclass_system, aarch64_system_sctlr+i,  2, 1); // data enable cache
                    regs->insert("sctlr_el"+n+".a",       aarch64_regclass_system, aarch64_system_sctlr+i,  1, 1); // alignment check enable bit
                    regs->insert("sctlr_el"+n+".m",       aarch64_regclass_system, aarch64_system_sctlr+i,  0, 1); // enable the MMU
                }
                regs->insert("tpidr_el"+n, aarch64_regclass_system, aarch64_system_tpidr+i, 0, 64); // user read/write thread ID register
            }
            if (i >= 1) {
                regs->insert("actlr_el"+n, aarch64_regclass_system, aarch64_system_actlr+i, 0, 64); // auxiliary control registers
                regs->insert("clidr_el"+n, aarch64_regclass_system, aarch64_system_clidr+i, 0, 64); // cache level ID registers
                regs->insert("elr_el"+n,   aarch64_regclass_system, aarch64_system_elr+i,   0, 64); // exception link registers
                regs->insert("esr_el"+n,   aarch64_regclass_system, aarch64_system_esr+i,   0, 64); // exception syndrome registers
                regs->insert("far_el"+n,   aarch64_regclass_system, aarch64_system_far+i,   0, 64); // fault address register
                regs->insert("mair_el"+n,  aarch64_regclass_system, aarch64_system_mair+i,  0, 64); // memory attribute indirection register
                //gs->insert("spsr_el"+n,  aarch64_regclass_system, aarch64_system_spsr+i,  0, 64); // saved program status register (see above)
                regs->insert("tcr_el"+n,   aarch64_regclass_system, aarch64_system_tcr+i,   0, 64); // translation control register
                regs->insert("ttbr0_el"+n, aarch64_regclass_system, aarch64_system_ttbr0+i, 0, 64); // translation table base register 0
                regs->insert("vbar_el"+n,  aarch64_regclass_system, aarch64_system_vbar+i,  0, 64); // vector based address register
            }
        }


        // "Advanced SIMD" registers, which are also the floating-point registers. [1. p. A3-77]
        // also known as "NEON and floating-point registers" [2, p. 4-16]
        for (unsigned i = 0; i < 32; ++i) {
            // Parts of vector registers
            for (size_t j = 0; j < 16; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".b[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 8 * j, 8);
            }
            for (size_t j = 0; j < 8; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".h[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 16 * j, 16);
            }
            for (size_t j = 0; j < 4; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".s[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 32 * j, 32);
            }
            for (size_t j = 0; j < 2; ++j) {
                regs->insert("v" + boost::lexical_cast<std::string>(i) + ".d[" + boost::lexical_cast<std::string>(j) + "]",
                             aarch64_regclass_ext, i, 64 * j, 64);
            }

            // Floating-point registers. 32 registers having names that access various parts of each register.
            regs->insert("v" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 128); // "quadword"
            regs->insert("d" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 64); // "doubleword"
            regs->insert("s" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 32); // "word"
            regs->insert("h" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 16); // "halfword"

            // Scalar registers are the same as the floating-point registers but use the name "q" to refer to the whole
            // register. It also uses "b" to refer to the least significant bit. Since the "q" and "v" registers are actually
            // the same register, and since ROSE identifies registers internally by their locations (RegisterDescriptor) rather
            // than their names, various parts of ROSE will report "q" instead of "v".
            regs->insert("q" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 128); // SIMD
            regs->insert("b" + boost::lexical_cast<std::string>(i), aarch64_regclass_ext, i, 0, 8); // SIMD
        }

        // Conditional execution registers. [1. p. A3-79]
        regs->insert("nzcv", aarch64_regclass_cc, 0, 0, 4);

        // Special registers
        regs->instructionPointerRegister("pc");
        regs->stackPointerRegister("sp");
        regs->stackFrameRegister("fp");
        regs->callReturnRegister("lr");
    }
    return regs;
}
#endif

#ifdef ROSE_ENABLE_ASM_AARCH32
RegisterDictionary::Ptr
RegisterDictionary::instanceAarch32() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("AArch32");

        // Pseudo register that returns a new free variable every time it's read.
        regs->insert("unknown", aarch32_regclass_sys, aarch32_sys_unknown, 0, 32);

        // 16 general purpose registers R0-R12, "sp" (stack pointer), "lr" (link register), and "pc" (program counter). The
        // stack pointer and link register are banked; that is, there are more than one stack register and link register but
        // only one is visible at a time depending on the current "system level view".
        for (unsigned i = 0; i < 13; ++i)
            regs->insert("r" + boost::lexical_cast<std::string>(i), aarch32_regclass_gpr, i, 0, 32);
        regs->insert("sb", aarch32_regclass_gpr, aarch32_gpr_sb, 0, 32); // alias for r9
        regs->insert("sl", aarch32_regclass_gpr, aarch32_gpr_sl, 0, 32); // alias for r10
        regs->insert("fp", aarch32_regclass_gpr, aarch32_gpr_fp, 0, 32); // alias for r11
        regs->insert("ip", aarch32_regclass_gpr, aarch32_gpr_ip, 0, 32); // alias for r12, not the instruction pointer (see PC)
        regs->insert("sp", aarch32_regclass_gpr, aarch32_gpr_sp, 0, 32); // the normal user stack pointer
        regs->insert("lr", aarch32_regclass_gpr, aarch32_gpr_lr, 0, 32); // the normal user link register
        regs->insert("pc", aarch32_regclass_gpr, aarch32_gpr_pc, 0, 32); // program counter, aka. insn pointer

        // Banked R8-R12
        regs->insert("r8_usr",  aarch32_regclass_sys, aarch32_sys_r8_usr,  0, 32);
        regs->insert("r8_fiq",  aarch32_regclass_sys, aarch32_sys_r8_fiq,  0, 32);
        regs->insert("r9_usr",  aarch32_regclass_sys, aarch32_sys_r9_usr,  0, 32);
        regs->insert("r9_fiq",  aarch32_regclass_sys, aarch32_sys_r9_fiq,  0, 32);
        regs->insert("r10_usr", aarch32_regclass_sys, aarch32_sys_r10_usr, 0, 32);
        regs->insert("r10_fiq", aarch32_regclass_sys, aarch32_sys_r10_fiq, 0, 32);
        regs->insert("r11_usr", aarch32_regclass_sys, aarch32_sys_r11_usr, 0, 32);
        regs->insert("r11_fiq", aarch32_regclass_sys, aarch32_sys_r11_fiq, 0, 32);
        regs->insert("r12_usr", aarch32_regclass_sys, aarch32_sys_r12_usr, 0, 32);
        regs->insert("r12_fiq", aarch32_regclass_sys, aarch32_sys_r12_fiq, 0, 32);

        // Banked stack pointers
        regs->insert("sp_usr", aarch32_regclass_sys, aarch32_sys_sp_usr, 0, 32);
        regs->insert("sp_hyp", aarch32_regclass_sys, aarch32_sys_sp_hyp, 0, 32);
        regs->insert("sp_svc", aarch32_regclass_sys, aarch32_sys_sp_svc, 0, 32);
        regs->insert("sp_abt", aarch32_regclass_sys, aarch32_sys_sp_abt, 0, 32);
        regs->insert("sp_und", aarch32_regclass_sys, aarch32_sys_sp_und, 0, 32);
        regs->insert("sp_mon", aarch32_regclass_sys, aarch32_sys_sp_mon, 0, 32);
        regs->insert("sp_irq", aarch32_regclass_sys, aarch32_sys_sp_irq, 0, 32);
        regs->insert("sp_fiq", aarch32_regclass_sys, aarch32_sys_sp_fiq, 0, 32);

        // Banked link pointers
        regs->insert("lr_usr", aarch32_regclass_sys, aarch32_sys_lr_usr, 0, 32);
        regs->insert("lr_svc", aarch32_regclass_sys, aarch32_sys_lr_svc, 0, 32);
        regs->insert("lr_abt", aarch32_regclass_sys, aarch32_sys_lr_abt, 0, 32);
        regs->insert("lr_und", aarch32_regclass_sys, aarch32_sys_lr_und, 0, 32);
        regs->insert("lr_mon", aarch32_regclass_sys, aarch32_sys_lr_mon, 0, 32);
        regs->insert("lr_irq", aarch32_regclass_sys, aarch32_sys_lr_irq, 0, 32);
        regs->insert("lr_fiq", aarch32_regclass_sys, aarch32_sys_lr_fiq, 0, 32);

        // One "cpsr" or "current program status register", which holds condition code flags, interrupt disable bits, current
        // processor mode, and other status and control information.
        regs->insert("cpsr",       aarch32_regclass_sys, aarch32_sys_cpsr, 0, 32);
        regs->insert("cpsr_nzcv",  aarch32_regclass_sys, aarch32_sys_cpsr, 28, 4); // n, z, c, and v bits
        regs->insert("cpsr_nzcvq", aarch32_regclass_sys, aarch32_sys_cpsr, 27, 5); // n, z, c, v, and q bits
        regs->insert("cpsr_n",     aarch32_regclass_sys, aarch32_sys_cpsr, 31, 1); // negative condition flag
        regs->insert("cpsr_z",     aarch32_regclass_sys, aarch32_sys_cpsr, 30, 1); // zero condition flag
        regs->insert("cpsr_c",     aarch32_regclass_sys, aarch32_sys_cpsr, 29, 1); // carry condition flag
        regs->insert("cpsr_v",     aarch32_regclass_sys, aarch32_sys_cpsr, 28, 1); // overflow condition flag
        regs->insert("cpsr_q",     aarch32_regclass_sys, aarch32_sys_cpsr, 27, 1); // cumulative saturation bit
        regs->insert("cpsr_j",     aarch32_regclass_sys, aarch32_sys_cpsr, 24, 1); // Java state
        regs->insert("cpsr_ssbs",  aarch32_regclass_sys, aarch32_sys_cpsr, 23, 1); // speculative store bypass safe (ARMv8.0-SSBS)
        regs->insert("cpsr_pan",   aarch32_regclass_sys, aarch32_sys_cpsr, 22, 1); // privileged access never (ARMv8.1-PAN)
        regs->insert("cpsr_dit",   aarch32_regclass_sys, aarch32_sys_cpsr, 21, 1); // data independent timing (ARMv8.4-DIT)
        regs->insert("cpsr_ge",    aarch32_regclass_sys, aarch32_sys_cpsr, 16, 4); // greater than or equal flags for || add sub
        regs->insert("cpsr_e",     aarch32_regclass_sys, aarch32_sys_cpsr, 9, 1);  // endianness state bit
        regs->insert("cpsr_a",     aarch32_regclass_sys, aarch32_sys_cpsr, 8, 1);  // SError interrupt mask bit
        regs->insert("cpsr_i",     aarch32_regclass_sys, aarch32_sys_cpsr, 7, 1);  // IRQ mask bit
        regs->insert("cpsr_f",     aarch32_regclass_sys, aarch32_sys_cpsr, 6, 1);  // FIQ mask bit
        regs->insert("cpsr_t",     aarch32_regclass_sys, aarch32_sys_cpsr, 5, 1);  // Thumb
        regs->insert("cpsr_m",     aarch32_regclass_sys, aarch32_sys_cpsr, 0, 4);  // current PE mode

        // These CPSR parts have special names in ARM assembly that conflict with the fields listed above. ROSE names the fields
        // using underscores for all architectures and tries to produce a consistent assembly style across all architectures. This
        // means that the names used by the ARM assembler, which conflict with the names above, need to be changed. We'll choose
        // something more descriptive than single letters.
        regs->insert("cpsr_control",   aarch32_regclass_sys, aarch32_sys_cpsr,  0, 8); // control bits, called "CPSR_c" in ARM assembly,
        regs->insert("cpsr_extension", aarch32_regclass_sys, aarch32_sys_cpsr,  8, 8); // extension bits, called "CPSR_x" in ARM assembly.
        regs->insert("cpsr_status",    aarch32_regclass_sys, aarch32_sys_cpsr, 16, 8); // status bits, called "CPSR_s" in ARM assembly.
        regs->insert("cpsr_flags",     aarch32_regclass_sys, aarch32_sys_cpsr, 24, 8); // flag bits N, Z, C, and V, called "CPSR_f" in ARM

        // Holds program status and control information, a subset of the CPSR.
        regs->insert("apsr",        aarch32_regclass_sys, aarch32_sys_apsr, 0, 32);
        regs->insert("apsr_nzcv",   aarch32_regclass_sys, aarch32_sys_apsr, 28, 4); // n, z, c, and v bits
        regs->insert("apsr_nzcvq",  aarch32_regclass_sys, aarch32_sys_apsr, 27, 5); // n, z, c, v, and q bits
        regs->insert("apsr_n",      aarch32_regclass_sys, aarch32_sys_apsr, 31, 1);
        regs->insert("apsr_z",      aarch32_regclass_sys, aarch32_sys_apsr, 30, 1);
        regs->insert("apsr_c",      aarch32_regclass_sys, aarch32_sys_apsr, 29, 1);
        regs->insert("apsr_v",      aarch32_regclass_sys, aarch32_sys_apsr, 28, 1);
        regs->insert("apsr_q",      aarch32_regclass_sys, aarch32_sys_apsr, 27, 1);
        regs->insert("apsr_ge",     aarch32_regclass_sys, aarch32_sys_apsr, 16, 4);

        // banked SPSR "saved program status register". The SPSR stores the value of the CPSR "current program status register"
        // when an exception is taken so that it can be restored after handling the exception.  Each exception handling mode
        // can access its own SPSR. User mode and System mode do not have an SPSR because they are not exception handling
        // states.
        regs->insert("spsr_hyp", aarch32_regclass_sys, aarch32_sys_spsr_hyp, 0, 32);
        regs->insert("spsr_svc", aarch32_regclass_sys, aarch32_sys_spsr_svc, 0, 32);
        regs->insert("spsr_abt", aarch32_regclass_sys, aarch32_sys_spsr_abt, 0, 32);
        regs->insert("spsr_und", aarch32_regclass_sys, aarch32_sys_spsr_und, 0, 32);
        regs->insert("spsr_mon", aarch32_regclass_sys, aarch32_sys_spsr_mon, 0, 32);
        regs->insert("spsr_irq", aarch32_regclass_sys, aarch32_sys_spsr_irq, 0, 32);
        regs->insert("spsr_fiq", aarch32_regclass_sys, aarch32_sys_spsr_fiq, 0, 32);

        // When an instruction is being decoded and the AST is being produced, we don't know what exception handling mode the
        // processor will be in when the instruction is executed. Therefore, ROSE creates a special "spsr" register whose read
        // and write operations will require translation to the correct SPSR hardware register later.
        regs->insert("spsr",           aarch32_regclass_sys, aarch32_sys_spsr,  0, 32);
        regs->insert("spsr_control",   aarch32_regclass_sys, aarch32_sys_spsr,  0,  8); // control bits, called "SPSR_c" in ARM assembly,
        regs->insert("spsr_extension", aarch32_regclass_sys, aarch32_sys_spsr,  8,  8); // extension bits, called "SPSR_x" in ARM assembly.
        regs->insert("spsr_status",    aarch32_regclass_sys, aarch32_sys_spsr, 16,  8); // status bits, called "SPSR_s" in ARM assembly.
        regs->insert("spsr_flags",     aarch32_regclass_sys, aarch32_sys_spsr, 24,  8); // flag bits N, Z, C, and V, called "SPSR_f" in ARM

        // I don't know what these are, but they're vaguely documented for the MSR instruction.
        regs->insert("ipsr",    aarch32_regclass_sys, aarch32_sys_ipsr,    0, 32);
        regs->insert("iepsr",   aarch32_regclass_sys, aarch32_sys_iepsr,   0, 32);
        regs->insert("iapsr",   aarch32_regclass_sys, aarch32_sys_iapsr,   0, 32);
        regs->insert("eapsr",   aarch32_regclass_sys, aarch32_sys_eapsr,   0, 32);
        regs->insert("psr",     aarch32_regclass_sys, aarch32_sys_psr,     0, 32);
        regs->insert("msp",     aarch32_regclass_sys, aarch32_sys_msp,     0, 32);
        regs->insert("psp",     aarch32_regclass_sys, aarch32_sys_psp,     0, 32);
        regs->insert("primask", aarch32_regclass_sys, aarch32_sys_primask, 0, 32);
        regs->insert("control", aarch32_regclass_sys, aarch32_sys_control, 0, 32);

        // VFP11 system registers. The VFPv2 architecture describes the following three system registers that must be present
        // in a VFP system.
        regs->insert("fpsid",   aarch32_regclass_sys, aarch32_sys_fpsid, 0, 32); // floating-point system ID register
        regs->insert("fpscr",   aarch32_regclass_sys, aarch32_sys_fpscr, 0, 32); // floating-point status and control register
        regs->insert("fpexc",   aarch32_regclass_sys, aarch32_sys_fpexc, 0, 32); // floating-point exception register

        // Various fields of SPSCR
        regs->insert("fpscr_n",      aarch32_regclass_sys, aarch32_sys_fpscr, 31, 1); // set if less than
        regs->insert("fpscr_z",      aarch32_regclass_sys, aarch32_sys_fpscr, 30, 1); // set if equal
        regs->insert("fpscr_c",      aarch32_regclass_sys, aarch32_sys_fpscr, 29, 1); // set if equal, greater than, or unordered
        regs->insert("fpscr_v",      aarch32_regclass_sys, aarch32_sys_fpscr, 28, 1); // set if unordered
        regs->insert("fpscr_nzcv",   aarch32_regclass_sys, aarch32_sys_fpscr, 28, 4); // N, Z, C, and V bits
        regs->insert("fpscr_dn",     aarch32_regclass_sys, aarch32_sys_fpscr, 25, 1); // default NaN mode enable bit
        regs->insert("fpscr_fz",     aarch32_regclass_sys, aarch32_sys_fpscr, 24, 1); // flush-to-zero mode enable bit
        regs->insert("fpscr_rmode",  aarch32_regclass_sys, aarch32_sys_fpscr, 22, 2); // rounding mode control
        regs->insert("fpscr_stride", aarch32_regclass_sys, aarch32_sys_fpscr, 20, 2); // vector length and stride control
        regs->insert("fpscr_len",    aarch32_regclass_sys, aarch32_sys_fpscr, 16, 3); // vector length and stride control
        regs->insert("fpscr_ide",    aarch32_regclass_sys, aarch32_sys_fpscr, 15, 1); // input subnormal exception enable
        regs->insert("fpscr_ixe",    aarch32_regclass_sys, aarch32_sys_fpscr, 12, 1); // inexact exception enable
        regs->insert("fpscr_ufe",    aarch32_regclass_sys, aarch32_sys_fpscr, 11, 1); // underflow exception enable
        regs->insert("fpscr_ofe",    aarch32_regclass_sys, aarch32_sys_fpscr, 10, 1); // overflow exception enable
        regs->insert("fpscr_dze",    aarch32_regclass_sys, aarch32_sys_fpscr,  9, 1); // division by zero exception enable
        regs->insert("fpscr_ioe",    aarch32_regclass_sys, aarch32_sys_fpscr,  8, 1); // invalid operation exception enable
        regs->insert("fpscr_idc",    aarch32_regclass_sys, aarch32_sys_fpscr,  7, 1); // input subnormal cumulative flag
        regs->insert("fpscr_ixc",    aarch32_regclass_sys, aarch32_sys_fpscr,  4, 1); // inexact cumulative flag
        regs->insert("fpscr_ufc",    aarch32_regclass_sys, aarch32_sys_fpscr,  3, 1); // underflow cumulative flag
        regs->insert("fpscr_ofc",    aarch32_regclass_sys, aarch32_sys_fpscr,  2, 1); // overflow cumulative flag
        regs->insert("fpscr_dzc",    aarch32_regclass_sys, aarch32_sys_fpscr,  1, 1); // division by zero cumulative flag
        regs->insert("fpscr_ioc",    aarch32_regclass_sys, aarch32_sys_fpscr,  0, 1); // invalid operation cumulative flag

        // To support exceptional conditions, the VFP11 coprocessor provides two additional registers. These registers
        // are designed to be used with the support code software available from ARM Limited. As a result, this
        // documentation [from ARM] does not fully specify exception handling in all cases.
        regs->insert("fpinst",  aarch32_regclass_sys, aarch32_sys_fpinst,  0, 32); // floating-point instruction register
        regs->insert("fpinst2", aarch32_regclass_sys, aarch32_sys_fpinst2, 0, 32); // floatinglcpoint instruction register two

        // The VFP11 coprocessor also provides two feature registers.
        regs->insert("mvfr0",   aarch32_regclass_sys, aarch32_sys_mvfr0, 0, 32); // media and VFP feature register 0
        regs->insert("mvfr1",   aarch32_regclass_sys, aarch32_sys_mvfr1, 0, 32); // media and VFP feature register 1

        // MVFR2, mediao, and VFP feature register 2. Describes the features provided by the AArch32 Advanced SIMD and
        // Floating-point implementation. Must be interpreted with MVFR0 and MVFR1. This register is present only when
        // AArch32 is supported at any exception level. Otherwise, direct accesses to MVFR2 are undefined. Implemented
        // only if the implementation includes Advanced SIMD and floating-point instructions.
        regs->insert("mvfr2",   aarch32_regclass_sys, aarch32_sys_mvfr2, 0, 32);

        // Thumb IT instructions
        regs->insert("itstate", aarch32_regclass_sys, aarch32_sys_itstate, 0, 32);

        // NEON and VFP use the same extension register bank. This is distinct from the ARM register bank. The extension
        // register bank is a colleciton of registers which can be accesed as either 32-bit, 64-bit, or 128-bit registers,
        // depending on whether the instruction is NEON or VFP.
        //
        // VFP views of the extension register bank. In VFPv3 and VFPv3-FP16 you can view the extension register bank as:
        //   * Thirty-two 64-bit registers, D0-D31
        //   * Thirty-two 32-bit registers, S0-S31. Only half of the register bank is accessible in this view.
        //   * A combination of registers from teh above views.
        //
        // In VFPv2, VFPv3-D16, and VFPv3-D16-FP16, you can view the extension reigster bank as:
        //   * Sixteen 64-bit registers, D0-D15
        //   * Thirty-two 32-bit registers, S0-S31
        //   A A combination of registers from the above views
        //
        // In VFP, 64-bit registers are called double-precison registers and can contain double-precision floating-point
        // values. 32-bit registers are called single-precision registers and can contain either a single-precision or two
        // half-precision floating-point values.
        //
        for (size_t i = 0; i < 32; ++i) {
            regs->insert("q" + boost::lexical_cast<std::string>(i), aarch32_regclass_ext, i, 0, 128);
            regs->insert("d" + boost::lexical_cast<std::string>(i), aarch32_regclass_ext, i, 0, 64);
            regs->insert("s" + boost::lexical_cast<std::string>(i), aarch32_regclass_ext, i, 0, 32);
        }

        // Coprocessor registers named "cr0" through "cr15", although these names don't actually appear in the documentation.
        for (unsigned i = 0; i < 16; ++i)
            regs->insert("cr" + boost::lexical_cast<std::string>(i), aarch32_regclass_coproc, i, 0, 32);

        // Debug registers. There are actually up to 1024 of these registers, but I'm not clear on how they're named.
        regs->insert("didr",  aarch32_regclass_debug, aarch32_debug_didr,  0, 32);
        regs->insert("wfar",  aarch32_regclass_debug, aarch32_debug_wfar,  0, 32);
        regs->insert("vcr",   aarch32_regclass_debug, aarch32_debug_vcr,   0, 32);
        regs->insert("ecr",   aarch32_regclass_debug, aarch32_debug_ecr,   0, 32);
        regs->insert("dsccr", aarch32_regclass_debug, aarch32_debug_dsccr, 0, 32);
        regs->insert("dsmcr", aarch32_regclass_debug, aarch32_debug_dsmcr, 0, 32);
        regs->insert("dtrrx", aarch32_regclass_debug, aarch32_debug_dtrrx, 0, 32);
        regs->insert("itr",   aarch32_regclass_debug, aarch32_debug_itr,   0, 32);
        regs->insert("dscr",  aarch32_regclass_debug, aarch32_debug_dscr,  0, 32);
        regs->insert("dtrtx", aarch32_regclass_debug, aarch32_debug_dtrtx, 0, 32);
        regs->insert("drcr",  aarch32_regclass_debug, aarch32_debug_drcr,  0, 32);
        for (unsigned i = 0; i < 16; ++i) {
            regs->insert("bvr" + boost::lexical_cast<std::string>(i), aarch32_regclass_debug, aarch32_debug_bvr0+i, 0, 32);
            regs->insert("bcr" + boost::lexical_cast<std::string>(i), aarch32_regclass_debug, aarch32_debug_bcr0+i, 0, 32);
            regs->insert("wvr" + boost::lexical_cast<std::string>(i), aarch32_regclass_debug, aarch32_debug_wvr0+i, 0, 32);
            regs->insert("wcr" + boost::lexical_cast<std::string>(i), aarch32_regclass_debug, aarch32_debug_wcr0+i, 0, 32);
        }
        regs->insert("oslar", aarch32_regclass_debug, aarch32_debug_oslar, 0, 32);
        regs->insert("oslsr", aarch32_regclass_debug, aarch32_debug_oslsr, 0, 32);
        regs->insert("ossrr", aarch32_regclass_debug, aarch32_debug_ossrr, 0, 32);
        regs->insert("prcr",  aarch32_regclass_debug, aarch32_debug_prcr,  0, 32);
        regs->insert("prsr",  aarch32_regclass_debug, aarch32_debug_prsr,  0, 32);
        regs->insert("itctrl", aarch32_regclass_debug, aarch32_debug_itctrl, 0, 32);
        regs->insert("claimset", aarch32_regclass_debug, aarch32_debug_claimset, 0, 32);
        regs->insert("claimclr", aarch32_regclass_debug, aarch32_debug_claimclr, 0, 32);
        regs->insert("lar",   aarch32_regclass_debug, aarch32_debug_lar,   0, 32);
        regs->insert("lsr",   aarch32_regclass_debug, aarch32_debug_lsr,   0, 32);
        regs->insert("authstatus", aarch32_regclass_debug, aarch32_debug_authstatus, 0, 32);
        regs->insert("devid", aarch32_regclass_debug, aarch32_debug_devid, 0, 32);
        regs->insert("devtype", aarch32_regclass_debug, aarch32_debug_devtype, 0, 32);
        for (unsigned i = 0; i < 8; ++i) {
            regs->insert("peripheralid" + boost::lexical_cast<std::string>(i), aarch32_regclass_debug,
                         aarch32_debug_peripheralid0+i, 0, 32);
        }
        for (unsigned i = 0; i < 4; ++i) {
            regs->insert("componentid" + boost::lexical_cast<std::string>(i), aarch32_regclass_debug,
                         aarch32_debug_componentid0+i, 0, 32);
        }

        // Special registers
        regs->instructionPointerRegister("pc");
        regs->stackPointerRegister("sp");
        regs->stackFrameRegister("fp");
        regs->callReturnRegister("lr");
    }
    return regs;
}
#endif

RegisterDictionary::Ptr
RegisterDictionary::instancePowerpc32() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WARNING: PowerPC documentation numbers register bits in reverse of their power-of-two position. ROSE numbers bits
    //          according to their power of two, so that the bit corresponding to 2^i is said to be at position i in the
    //          reigster.  In PowerPC documentation, the bit for 2^i is at N - (i+1) where N is the total number of bits in the
    //          reigster.  All PowerPC bit position numbers need to be converted to the ROSE numbering when they appear here.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("powerpc-32");

        /**********************************************************************************************************************
         * General purpose and floating point registers
         **********************************************************************************************************************/
        for (unsigned i=0; i<32; i++) {
            regs->insert("r"+StringUtility::numberToString(i), powerpc_regclass_gpr, i, 0, 32);
            regs->insert("f"+StringUtility::numberToString(i), powerpc_regclass_fpr, i, 0, 64);
        }

        /**********************************************************************************************************************
         * State, status, condition, control registers
         **********************************************************************************************************************/

        /* Machine state register */
        regs->insert("msr", powerpc_regclass_msr, 0, 0, 32);

        /* Floating point status and control register */
        regs->insert("fpscr", powerpc_regclass_fpscr, 0, 0, 32);

        // Condition Register. This register is grouped into eight fields, where each field is 4 bits. Many PowerPC
        // instructions define the least significant bit of the instruction encoding as the Rc bit, and some instructions imply
        // an Rc value equal to 1. When Rc is equal to 1 for integer operations, the CR0 field is set to reflect the result of
        // the instruction's operation: Less than zero (LT), greater than zero (GT), equal to zero (EQ), and summary overflow
        // (SO). When Rc is equal to 1 for floating-point operations, the CR1 field is set to reflect the state of the
        // exception status bits in the FPSCR: FX, FEX, VX, and OX. Any CR field can be the target of an integer or
        // floating-point comparison instruction. The CR0 field is also set to reflect the result of a conditional store
        // instruction (stwcx or stdcx). There is also a set of instructions that can manipulate a specific CR bit, a specific
        // CR field, or the entire CR, usually to combine several conditions into a single bit for testing.
        regs->insert("cr",  powerpc_regclass_cr, 0,  0, 32);

        regs->insert("cr0", powerpc_regclass_cr, 0, 28,  4);
        regs->insert("cr0.lt", powerpc_regclass_cr, 0, 31, 1);
        regs->insert("cr0.gt", powerpc_regclass_cr, 0, 30, 1);
        regs->insert("cr0.eq", powerpc_regclass_cr, 0, 29, 1);
        regs->insert("cr0.so", powerpc_regclass_cr, 0, 28, 1);

        regs->insert("cr1", powerpc_regclass_cr, 0, 24,  4);
        regs->insert("cr1.lt", powerpc_regclass_cr, 0, 27, 1);
        regs->insert("cr1.gt", powerpc_regclass_cr, 0, 26, 1);
        regs->insert("cr1.eq", powerpc_regclass_cr, 0, 25, 1);
        regs->insert("cr1.so", powerpc_regclass_cr, 0, 24, 1);

        regs->insert("cr2", powerpc_regclass_cr, 0, 20,  4);
        regs->insert("cr2.lt", powerpc_regclass_cr, 0, 23, 1);
        regs->insert("cr2.gt", powerpc_regclass_cr, 0, 22, 1);
        regs->insert("cr2.eq", powerpc_regclass_cr, 0, 21, 1);
        regs->insert("cr2.so", powerpc_regclass_cr, 0, 20, 1);

        regs->insert("cr3", powerpc_regclass_cr, 0, 16,  4);
        regs->insert("cr3.lt", powerpc_regclass_cr, 0, 19, 1);
        regs->insert("cr3.gt", powerpc_regclass_cr, 0, 18, 1);
        regs->insert("cr3.eq", powerpc_regclass_cr, 0, 17, 1);
        regs->insert("cr3.so", powerpc_regclass_cr, 0, 16, 1);

        regs->insert("cr4", powerpc_regclass_cr, 0, 12,  4);
        regs->insert("cr4.lt", powerpc_regclass_cr, 0, 15, 1);
        regs->insert("cr4.gt", powerpc_regclass_cr, 0, 14, 1);
        regs->insert("cr4.eq", powerpc_regclass_cr, 0, 13, 1);
        regs->insert("cr4.so", powerpc_regclass_cr, 0, 12, 1);

        regs->insert("cr5", powerpc_regclass_cr, 0,  8,  4);
        regs->insert("cr5.lt", powerpc_regclass_cr, 0, 11, 1);
        regs->insert("cr5.gt", powerpc_regclass_cr, 0, 10, 1);
        regs->insert("cr5.eq", powerpc_regclass_cr, 0,  9, 1);
        regs->insert("cr5.so", powerpc_regclass_cr, 0,  8, 1);

        regs->insert("cr6", powerpc_regclass_cr, 0,  4,  4);
        regs->insert("cr6.lt", powerpc_regclass_cr, 0,  7, 1);
        regs->insert("cr6.gt", powerpc_regclass_cr, 0,  6, 1);
        regs->insert("cr6.eq", powerpc_regclass_cr, 0,  5, 1);
        regs->insert("cr6.so", powerpc_regclass_cr, 0,  4, 1);

        regs->insert("cr7", powerpc_regclass_cr, 0,  0,  4);
        regs->insert("cr7.lt", powerpc_regclass_cr, 0,  3, 1);
        regs->insert("cr7.gt", powerpc_regclass_cr, 0,  2, 1);
        regs->insert("cr7.eq", powerpc_regclass_cr, 0,  1, 1);
        regs->insert("cr7.so", powerpc_regclass_cr, 0,  0, 1);

        /* The processor version register is a 32-bit read-only register that identifies the version and revision level of the
         * processor. Processor versions are assigned by the PowerPC architecture process. Revision levels are implementation
         * defined. Access to the register is privileged, so that an application program can determine the processor version
         * only with the help of an operating system function. */
        regs->insert("pvr", powerpc_regclass_pvr, 0, 0, 32);

        /**********************************************************************************************************************
         * The instruction address register is a pseudo register. It is not directly available to the user other than through a
         * "branch and link" instruction. It is primarily used by debuggers to show the next instruction to be executed.
         **********************************************************************************************************************/
        regs->insert("iar", powerpc_regclass_iar, 0, 0, 32);

        /**********************************************************************************************************************
         * Special purpose registers. There are 1024 of these, some of which have special names.  We name all 1024 consistently
         * and create aliases for the special ones. This allows the disassembler to look them up generically.  Because the
         * special names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        /* Generic names for them all */
        for (unsigned i=0; i<1024; i++)
            regs->insert("spr"+StringUtility::numberToString(i), powerpc_regclass_spr, i, 0, 32);

        /* The link register contains the address to return to at the end of a function call.  Each branch instruction encoding
         * has an LK bit. If the LK bit is 1, the branch instruction moves the program counter to the link register. Also, the
         * conditional branch instruction BCLR branches to the value in the link register. */
        regs->insert("lr", powerpc_regclass_spr, powerpc_spr_lr, 0, 32);

        /* The fixed-point exception register contains carry and overflow information from integer arithmetic operations. It
         * also contains carry input to certain integer arithmetic operations and the number of bytes to transfer during load
         * and store string instructions, lswx and stswx. */
        regs->insert("xer", powerpc_regclass_spr, powerpc_spr_xer, 0, 32);
        regs->insert("xer_so", powerpc_regclass_spr, powerpc_spr_xer, 31, 1); // summary overflow
        regs->insert("xer_ov", powerpc_regclass_spr, powerpc_spr_xer, 30, 1); // overflow
        regs->insert("xer_ca", powerpc_regclass_spr, powerpc_spr_xer, 29, 1); // carry

        /* The count register contains a loop counter that is decremented on certain branch operations. Also, the conditional
         * branch instruction bcctr branches to the value in the CTR. */
        regs->insert("ctr", powerpc_regclass_spr, powerpc_spr_ctr, 0, 32);

        /* Other special purpose registers. */
        regs->insert("dsisr", powerpc_regclass_spr, powerpc_spr_dsisr, 0, 32);
        regs->insert("dar", powerpc_regclass_spr, powerpc_spr_dar, 0, 32);
        regs->insert("dec", powerpc_regclass_spr, powerpc_spr_dec, 0, 32);

        /**********************************************************************************************************************
         * Time base registers. There are 1024 of these, some of which have special names. We name all 1024 consistently and
         * create aliases for the special ones. This allows the disassembler to look them up generically.  Because the special
         * names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        for (unsigned i=0; i<1024; i++)
            regs->insert("tbr"+StringUtility::numberToString(i), powerpc_regclass_tbr, i, 0, 32);

        regs->insert("tbl", powerpc_regclass_tbr, powerpc_tbr_tbl, 0, 32);      /* time base lower */
        regs->insert("tbu", powerpc_regclass_tbr, powerpc_tbr_tbu, 0, 32);      /* time base upper */

        // Special registers
        regs->instructionPointerRegister("iar");
        regs->stackPointerRegister("r1");
        regs->stackFrameRegister("r31");
        regs->callReturnRegister("lr");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instancePowerpc64() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WARNING: PowerPC documentation numbers register bits in reverse of their power-of-two position. ROSE numbers bits
    //          according to their power of two, so that the bit corresponding to 2^i is said to be at position i in the
    //          reigster.  In PowerPC documentation, the bit for 2^i is at N - (i+1) where N is the total number of bits in the
    //          reigster.  All PowerPC bit position numbers need to be converted to the ROSE numbering when they appear here.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("powerpc-64");

        /**********************************************************************************************************************
         * General purpose and floating point registers
         **********************************************************************************************************************/
        for (unsigned i=0; i<32; i++) {
            regs->insert("r"+StringUtility::numberToString(i), powerpc_regclass_gpr, i, 0, 64);
            regs->insert("f"+StringUtility::numberToString(i), powerpc_regclass_fpr, i, 0, 64);
        }

        /**********************************************************************************************************************
         * State, status, condition, control registers
         **********************************************************************************************************************/

        /* Machine state register */
        regs->insert("msr", powerpc_regclass_msr, 0, 0, 32);

        /* Floating point status and control register */
        regs->insert("fpscr", powerpc_regclass_fpscr, 0, 0, 32);

        // Condition Register. This register is grouped into eight fields, where each field is 4 bits. Many PowerPC
        // instructions define the least significant bit of the instruction encoding as the Rc bit, and some instructions imply
        // an Rc value equal to 1. When Rc is equal to 1 for integer operations, the CR0 field is set to reflect the result of
        // the instruction's operation: Less than zero (LT), greater than zero (GT), equal to zero (EQ), and summary overflow
        // (SO). When Rc is equal to 1 for floating-point operations, the CR1 field is set to reflect the state of the
        // exception status bits in the FPSCR: FX, FEX, VX, and OX. Any CR field can be the target of an integer or
        // floating-point comparison instruction. The CR0 field is also set to reflect the result of a conditional store
        // instruction (stwcx or stdcx). There is also a set of instructions that can manipulate a specific CR bit, a specific
        // CR field, or the entire CR, usually to combine several conditions into a single bit for testing.
        regs->insert("cr",  powerpc_regclass_cr, 0,  0, 32);

        regs->insert("cr0", powerpc_regclass_cr, 0, 28,  4);
        regs->insert("cr0.lt", powerpc_regclass_cr, 0, 31, 1);
        regs->insert("cr0.gt", powerpc_regclass_cr, 0, 30, 1);
        regs->insert("cr0.eq", powerpc_regclass_cr, 0, 29, 1);
        regs->insert("cr0.so", powerpc_regclass_cr, 0, 28, 1);

        regs->insert("cr1", powerpc_regclass_cr, 0, 24,  4);
        regs->insert("cr1.lt", powerpc_regclass_cr, 0, 27, 1);
        regs->insert("cr1.gt", powerpc_regclass_cr, 0, 26, 1);
        regs->insert("cr1.eq", powerpc_regclass_cr, 0, 25, 1);
        regs->insert("cr1.so", powerpc_regclass_cr, 0, 24, 1);

        regs->insert("cr2", powerpc_regclass_cr, 0, 20,  4);
        regs->insert("cr2.lt", powerpc_regclass_cr, 0, 23, 1);
        regs->insert("cr2.gt", powerpc_regclass_cr, 0, 22, 1);
        regs->insert("cr2.eq", powerpc_regclass_cr, 0, 21, 1);
        regs->insert("cr2.so", powerpc_regclass_cr, 0, 20, 1);

        regs->insert("cr3", powerpc_regclass_cr, 0, 16,  4);
        regs->insert("cr3.lt", powerpc_regclass_cr, 0, 19, 1);
        regs->insert("cr3.gt", powerpc_regclass_cr, 0, 18, 1);
        regs->insert("cr3.eq", powerpc_regclass_cr, 0, 17, 1);
        regs->insert("cr3.so", powerpc_regclass_cr, 0, 16, 1);

        regs->insert("cr4", powerpc_regclass_cr, 0, 12,  4);
        regs->insert("cr4.lt", powerpc_regclass_cr, 0, 15, 1);
        regs->insert("cr4.gt", powerpc_regclass_cr, 0, 14, 1);
        regs->insert("cr4.eq", powerpc_regclass_cr, 0, 13, 1);
        regs->insert("cr4.so", powerpc_regclass_cr, 0, 12, 1);

        regs->insert("cr5", powerpc_regclass_cr, 0,  8,  4);
        regs->insert("cr5.lt", powerpc_regclass_cr, 0, 11, 1);
        regs->insert("cr5.gt", powerpc_regclass_cr, 0, 10, 1);
        regs->insert("cr5.eq", powerpc_regclass_cr, 0,  9, 1);
        regs->insert("cr5.so", powerpc_regclass_cr, 0,  8, 1);

        regs->insert("cr6", powerpc_regclass_cr, 0,  4,  4);
        regs->insert("cr6.lt", powerpc_regclass_cr, 0,  7, 1);
        regs->insert("cr6.gt", powerpc_regclass_cr, 0,  6, 1);
        regs->insert("cr6.eq", powerpc_regclass_cr, 0,  5, 1);
        regs->insert("cr6.so", powerpc_regclass_cr, 0,  4, 1);

        regs->insert("cr7", powerpc_regclass_cr, 0,  0,  4);
        regs->insert("cr7.lt", powerpc_regclass_cr, 0,  3, 1);
        regs->insert("cr7.gt", powerpc_regclass_cr, 0,  2, 1);
        regs->insert("cr7.eq", powerpc_regclass_cr, 0,  1, 1);
        regs->insert("cr7.so", powerpc_regclass_cr, 0,  0, 1);

        /* The processor version register is a 32-bit read-only register that identifies the version and revision level of the
         * processor. Processor versions are assigned by the PowerPC architecture process. Revision levels are implementation
         * defined. Access to the register is privileged, so that an application program can determine the processor version
         * only with the help of an operating system function. */
        regs->insert("pvr", powerpc_regclass_pvr, 0, 0, 32);

        /**********************************************************************************************************************
         * The instruction address register is a pseudo register. It is not directly available to the user other than through a
         * "branch and link" instruction. It is primarily used by debuggers to show the next instruction to be executed.
         **********************************************************************************************************************/
        regs->insert("iar", powerpc_regclass_iar, 0, 0, 64);

        /**********************************************************************************************************************
         * Special purpose registers. There are 1024 of these, some of which have special names.  We name all 1024 consistently
         * and create aliases for the special ones. This allows the disassembler to look them up generically.  Because the
         * special names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        /* Generic names for them all */
        for (unsigned i=0; i<1024; i++)
            regs->insert("spr"+StringUtility::numberToString(i), powerpc_regclass_spr, i, 0, 64);

        /* The link register contains the address to return to at the end of a function call.  Each branch instruction encoding
         * has an LK bit. If the LK bit is 1, the branch instruction moves the program counter to the link register. Also, the
         * conditional branch instruction BCLR branches to the value in the link register. */
        regs->insert("lr", powerpc_regclass_spr, powerpc_spr_lr, 0, 64);

        /* The fixed-point exception register contains carry and overflow information from integer arithmetic operations. It
         * also contains carry input to certain integer arithmetic operations and the number of bytes to transfer during load
         * and store string instructions, lswx and stswx. */
        regs->insert("xer", powerpc_regclass_spr, powerpc_spr_xer, 0, 64);
        regs->insert("xer_so", powerpc_regclass_spr, powerpc_spr_xer, 31, 1); // summary overflow
        regs->insert("xer_ov", powerpc_regclass_spr, powerpc_spr_xer, 30, 1); // overflow
        regs->insert("xer_ca", powerpc_regclass_spr, powerpc_spr_xer, 29, 1); // carry

        /* The count register contains a loop counter that is decremented on certain branch operations. Also, the conditional
         * branch instruction bcctr branches to the value in the CTR. */
        regs->insert("ctr", powerpc_regclass_spr, powerpc_spr_ctr, 0, 64);

        /* Other special purpose registers. */
        regs->insert("dsisr", powerpc_regclass_spr, powerpc_spr_dsisr, 0, 64);
        regs->insert("dar", powerpc_regclass_spr, powerpc_spr_dar, 0, 64);
        regs->insert("dec", powerpc_regclass_spr, powerpc_spr_dec, 0, 64);

        /**********************************************************************************************************************
         * Time base registers. There are 1024 of these, some of which have special names. We name all 1024 consistently and
         * create aliases for the special ones. This allows the disassembler to look them up generically.  Because the special
         * names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        for (unsigned i=0; i<1024; i++)
            regs->insert("tbr"+StringUtility::numberToString(i), powerpc_regclass_tbr, i, 0, 64);

        regs->insert("tbl", powerpc_regclass_tbr, powerpc_tbr_tbl, 0, 64);      /* time base lower */
        regs->insert("tbu", powerpc_regclass_tbr, powerpc_tbr_tbu, 0, 64);      /* time base upper */

        // Special registers
        regs->instructionPointerRegister("iar");
        regs->stackPointerRegister("r1");
        regs->stackFrameRegister("r31");
        regs->callReturnRegister("lr");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceMips32() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("mips32");

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
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceMips32AlternateNames() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("mips32");
        regs->insert(instanceMips32());

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
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceM68000() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("m68000");

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

        // Special registers
        regs->instructionPointerRegister("pc");
        regs->stackPointerRegister("a7");
        regs->stackFrameRegister("a6");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceM68000AlternateNames() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("m68000");
        regs->insert(instanceM68000());
        regs->insert("fp", m68k_regclass_addr, 6, 0, 32);                       // a6 is conventionally the stack frame pointer
        regs->insert("sp", m68k_regclass_addr, 7, 0, 32);                       // a7 is conventionally the stack pointer
    }
    return regs;
}

// FIXME[Robb P. Matzke 2014-07-15]: This is fairly generic at this point. Eventually we'll split this function into
// dictionaries for each specific Freescale ColdFire architecture.
RegisterDictionary::Ptr
RegisterDictionary::instanceColdfire() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("freescale MAC");
        regs->insert(instanceM68000AlternateNames());

        // Floating point data registers.
        // The ColdFire processors do not support extended precision real ("X") format values, and therefore don't need
        // full 80-bit floating point data registers.  Their FP data registers are only 64 bits.
        for (int i=0; i<8; ++i)
            regs->resize("fp"+StringUtility::numberToString(i), 64);

        // MAC unit
        regs->insert("macsr",     m68k_regclass_mac, m68k_mac_macsr, 0, 32);    // MAC status register
        regs->insert("macsr_c",   m68k_regclass_mac, m68k_mac_macsr, 0,  1);    //   Carry flag; this field is always zero.
        regs->insert("macsr_v",   m68k_regclass_mac, m68k_mac_macsr, 1,  1);    //   Overflow flag
        regs->insert("macsr_z",   m68k_regclass_mac, m68k_mac_macsr, 2,  1);    //   Zero flag
        regs->insert("macsr_n",   m68k_regclass_mac, m68k_mac_macsr, 3,  1);    //   Negative flag
        regs->insert("macsr_rt",  m68k_regclass_mac, m68k_mac_macsr, 4,  1);    //   Round/truncate mode
        regs->insert("macsr_fi",  m68k_regclass_mac, m68k_mac_macsr, 5,  1);    //   Fraction/integer mode
        regs->insert("macsr_su",  m68k_regclass_mac, m68k_mac_macsr, 6,  1);    //   Signed/unsigned operations mode
        regs->insert("macsr_omc", m68k_regclass_mac, m68k_mac_macsr, 7,  1);    //   Overflow/saturation mode
        regs->insert("acc",       m68k_regclass_mac, m68k_mac_acc0,  0, 32);    // MAC accumulator
        regs->insert("mask",      m68k_regclass_mac, m68k_mac_mask,  0, 32);    // MAC mask register (upper 16 bits are set)

        // Supervisor registers
        regs->insert("cacr",     m68k_regclass_sup, m68k_sup_cacr,      0, 32); // cache control register
        regs->insert("asid",     m68k_regclass_sup, m68k_sup_asid,      0, 32); // address space ID register
        regs->insert("acr0",     m68k_regclass_sup, m68k_sup_acr0,      0, 32); // access control register 0 (data)
        regs->insert("acr1",     m68k_regclass_sup, m68k_sup_acr1,      0, 32); // access control register 1 (data)
        regs->insert("acr2",     m68k_regclass_sup, m68k_sup_acr2,      0, 32); // access control register 2 (instruction)
        regs->insert("acr3",     m68k_regclass_sup, m68k_sup_acr3,      0, 32); // access control register 3 (instruction)
        regs->insert("mmubar",   m68k_regclass_sup, m68k_sup_mmubar,    0, 32); // MMU base address register
        regs->insert("rombar0",  m68k_regclass_sup, m68k_sup_rombar0,   0, 32); // ROM base address register 0
        regs->insert("rombar1",  m68k_regclass_sup, m68k_sup_rombar1,   0, 32); // ROM base address register 1
        regs->insert("rambar0",  m68k_regclass_sup, m68k_sup_rambar0,   0, 32); // RAM base address register 0
        regs->insert("rambar1",  m68k_regclass_sup, m68k_sup_rambar1,   0, 32); // RAM base address register 1
        regs->insert("mbar",     m68k_regclass_sup, m68k_sup_mbar,      0, 32); // module base address register
        regs->insert("mpcr",     m68k_regclass_sup, m68k_sup_mpcr,      0, 32); // multiprocessor control register
        regs->insert("edrambar", m68k_regclass_sup, m68k_sup_edrambar,  0, 32); // embedded DRAM base address register
        regs->insert("secmbar",  m68k_regclass_sup, m68k_sup_secmbar,   0, 32); // secondary module base address register
        regs->insert("pcr1u0",   m68k_regclass_sup, m68k_sup_0_pcr1,   32, 32); // 32 msbs of RAM 0 permutation control reg 1
        regs->insert("pcr1l0",   m68k_regclass_sup, m68k_sup_0_pcr1,    0, 32); // 32 lsbs of RAM 0 permutation control reg 1
        regs->insert("pcr2u0",   m68k_regclass_sup, m68k_sup_0_pcr2,   32, 32); // 32 msbs of RAM 0 permutation control reg 2
        regs->insert("pcr2l0",   m68k_regclass_sup, m68k_sup_0_pcr2,    0, 32); // 32 lsbs of RAM 0 permutation control reg 2
        regs->insert("pcr3u0",   m68k_regclass_sup, m68k_sup_0_pcr3,   32, 32); // 32 msbs of RAM 0 permutation control reg 3
        regs->insert("pcr3l0",   m68k_regclass_sup, m68k_sup_0_pcr3,    0, 32); // 32 lsbs of RAM 0 permutation control reg 3
        regs->insert("pcr1u1",   m68k_regclass_sup, m68k_sup_1_pcr1,   32, 32); // 32 msbs of RAM 1 permutation control reg 1
        regs->insert("pcr1l1",   m68k_regclass_sup, m68k_sup_1_pcr1,    0, 32); // 32 lsbs of RAM 1 permutation control reg 1
        regs->insert("pcr2u1",   m68k_regclass_sup, m68k_sup_1_pcr2,   32, 32); // 32 msbs of RAM 1 permutation control reg 2
        regs->insert("pcr2l1",   m68k_regclass_sup, m68k_sup_1_pcr2,    0, 32); // 32 lsbs of RAM 1 permutation control reg 2
        regs->insert("pcr3u1",   m68k_regclass_sup, m68k_sup_1_pcr3,   32, 32); // 32 msbs of RAM 1 permutation control reg 3
        regs->insert("pcr3l1",   m68k_regclass_sup, m68k_sup_1_pcr3,    0, 32); // 32 lsbs of RAM 1 permutation control reg 3
    }
    return regs;
}

// FreeScale ColdFire CPUs with EMAC (extended multiply-accumulate) unit.
RegisterDictionary::Ptr
RegisterDictionary::instanceColdfireEmac() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("freescale EMAC");
        regs->insert(instanceColdfire());

        regs->insert("macsr_pav0", m68k_regclass_mac, m68k_mac_macsr,  8,  1);  // overflow flag for accumulator 0
        regs->insert("macsr_pav1", m68k_regclass_mac, m68k_mac_macsr,  9,  1);  // overflow flag for accumulator 1
        regs->insert("macsr_pav2", m68k_regclass_mac, m68k_mac_macsr, 10,  1);  // overflow flag for accumulator 2
        regs->insert("macsr_pav3", m68k_regclass_mac, m68k_mac_macsr, 11,  1);  // overflow flag for accumulator 3

        regs->insert("acc0",       m68k_regclass_mac, m68k_mac_acc0,   0, 32);  // accumulator #0
        regs->insert("acc1",       m68k_regclass_mac, m68k_mac_acc1,   0, 32);  // accumulator #1
        regs->insert("acc2",       m68k_regclass_mac, m68k_mac_acc2,   0, 32);  // accumulator #2
        regs->insert("acc3",       m68k_regclass_mac, m68k_mac_acc3,   0, 32);  // accumulator #3

        regs->insert("accext01",   m68k_regclass_mac, m68k_mac_ext01,  0, 32);  // extensions for acc0 and acc1
        regs->insert("accext0",    m68k_regclass_mac, m68k_mac_ext0,   0, 16);
        regs->insert("accext1",    m68k_regclass_mac, m68k_mac_ext1,  16, 16);
        regs->insert("accext23",   m68k_regclass_mac, m68k_mac_ext23,  0, 32);  // extensions for acc2 and acc3
        regs->insert("accext2",    m68k_regclass_mac, m68k_mac_ext2,   0, 16);
        regs->insert("accext3",    m68k_regclass_mac, m68k_mac_ext3,  16, 16);
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceCil() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("Cil");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceJvm() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs = RegisterDictionary::instance("Jvm");
    }
    return regs;
}

RegisterDictionary::Ptr
RegisterDictionary::instanceForIsa(SgAsmExecutableFileFormat::InsSetArchitecture isa) {
    typedef SgAsmExecutableFileFormat EFF;
    switch (isa & EFF::ISA_FAMILY_MASK) {
        case EFF::ISA_IA32_Family:
            switch (isa) {
                case EFF::ISA_IA32_286:
                    return instanceI286();

                case EFF::ISA_IA32_386:
                case EFF::ISA_IA32_486:
                case EFF::ISA_IA32_Pentium:
                case EFF::ISA_IA32_Pentium4:
                    // The ROSE x86 disassembler uses Pentium4 registers always. This is because some compilers (such as nasm)
                    // set the ISA to 386 even for later 32-bit processors. In any case, Pentium4 has all the same registers as
                    // I386Math; it just has some extras.
                    return instancePentium4();

                default:
                    return instancePentium4();
            }
            break;

        case EFF::ISA_X8664_Family:
            return instanceAmd64();

        case EFF::ISA_MIPS_Family:
            return instanceMips32AlternateNames(); // disassembler assumes only mips32()

#ifdef ROSE_ENABLE_ASM_AARCH64
        case EFF::ISA_ARM_Family:
            return instanceAarch64();
#endif

        case EFF::ISA_PowerPC:
            return instancePowerpc32();

        case EFF::ISA_PowerPC_64bit:
            return instancePowerpc64();

        case EFF::ISA_M68K_Family:
            return instanceColdfire();

        default:
            return Ptr();
    }
}

RegisterDictionary::Ptr
RegisterDictionary::instanceForIsa(SgAsmInterpretation *interp) {
    const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
    return hdrs.empty() ? Ptr() : instanceForIsa(hdrs.front()->get_isa());
}

const std::string&
RegisterDictionary::name() const {
    return name_;
}

void
RegisterDictionary::name(const std::string &s) {
    name_ = s;
}

const RegisterDictionary::Entries &
RegisterDictionary::registers() const {
    return forward_;
}

void
RegisterDictionary::insert(const std::string &name, RegisterDescriptor reg) { //
    /* Erase the name from the reverse lookup map, indexed by the old descriptor. */
    Entries::iterator fi = forward_.find(name);
    if (fi != forward_.end()) {
        Reverse::iterator ri = reverse_.find(fi->second);
        ASSERT_require(ri != reverse_.end());
        std::vector<std::string>::iterator vi=std::find(ri->second.begin(), ri->second.end(), name);
        ASSERT_require(vi != ri->second.end());
        ri->second.erase(vi);
    }

    /* Insert or replace old descriptor with a new one and insert reverse lookup info. */
    forward_[name] = reg;
    reverse_[reg].push_back(name);
}

void
RegisterDictionary::insert(const std::string &name, unsigned majr, unsigned minr, unsigned offset, unsigned nbits) {
    insert(name, RegisterDescriptor(majr, minr, offset, nbits));
}

void
RegisterDictionary::insert(const Ptr &other) {
    ASSERT_not_null(other);
    const Entries &entries = other->registers();
    for (Entries::const_iterator ei = entries.begin(); ei != entries.end(); ++ei)
        insert(ei->first, ei->second);

    instructionPointer_ = other->instructionPointer_;
    stackPointer_ = other->stackPointer_;
    stackFrame_ = other->stackFrame_;
    stackSegment_ = other->stackSegment_;
    callReturn_ = other->callReturn_;
}

void
RegisterDictionary::resize(const std::string &name, unsigned new_nbits) {
    RegisterDescriptor reg = findOrThrow(name);
    reg.nBits(new_nbits);
    insert(name, reg);
}

RegisterDescriptor
RegisterDictionary::find(const std::string &name) const {
    Entries::const_iterator fi = forward_.find(name);
    return forward_.end() == fi ? RegisterDescriptor() : fi->second;
}

RegisterDescriptor
RegisterDictionary::findOrThrow(const std::string &name) const {
    Entries::const_iterator fi = forward_.find(name);
    if (forward_.end() == fi)
        throw Exception("register " + name + " not found");
    return fi->second;
}

const std::string&
RegisterDictionary::lookup(RegisterDescriptor reg) const {
    Reverse::const_iterator ri = reverse_.find(reg);
    if (ri != reverse_.end()) {
        for (size_t i = ri->second.size(); i > 0; --i) {
            const std::string &name = ri->second[i-1];
            Entries::const_iterator fi = forward_.find(name);
            ASSERT_require(fi != forward_.end());
            if (fi->second == reg)
                return name;
        }
    }

    static const std::string empty;
    return empty;
}

bool
RegisterDictionary::exists(RegisterDescriptor rdesc) const {
    Reverse::const_iterator found = reverse_.find(rdesc);
    return found == reverse_.end();
}
RegisterDescriptor
RegisterDictionary::findLargestRegister(unsigned major, unsigned minor, size_t maxWidth) const {
    RegisterDescriptor retval;
    for (Entries::const_iterator iter = forward_.begin(); iter != forward_.end(); ++iter) {
        RegisterDescriptor reg = iter->second;
        if (major == reg.majorNumber() && minor == reg.minorNumber()) {
            if (maxWidth > 0 && reg.nBits() > maxWidth) {
                // ignore
            } else if (retval.isEmpty()) {
                retval = reg;
            } else if (retval.nBits() < reg.nBits()) {
                retval = reg;
            }
        }
    }
    return retval;
}

RegisterParts
RegisterDictionary::getAllParts() const {
    RegisterParts retval;
    for (const Entries::value_type &node: forward_)
        retval.insert(node.second);
    return retval;
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::getDescriptors() const {
    const Entries &entries = registers();
    RegisterDescriptors retval;
    retval.reserve(entries.size());
    for (Entries::const_iterator ei = entries.begin(); ei != entries.end(); ++ei)
        retval.push_back(ei->second);
    return retval;
}

// Returns lowest possible non-negative value not present in v
static unsigned
firstUnused(std::vector<unsigned> &v /*in,out*/) {
    if (v.empty())
        return 0;
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    if (v.back() + 1 == v.size())
        return v.size();
    for (size_t i=0; i<v.size(); ++i) {
        if (v[i] != i)
            return i;
    }
    ASSERT_not_reachable("should have returned by now");
}

unsigned
RegisterDictionary::firstUnusedMajor() const {
    std::vector<unsigned> used;
    for (const Entries::value_type &entry: forward_) {
        if (used.empty() || used.back() != entry.second.majorNumber())
            used.push_back(entry.second.majorNumber());
    }
    return firstUnused(used);
}

unsigned
RegisterDictionary::firstUnusedMinor(unsigned majr) const {
    std::vector<unsigned> used;
    for (const Entries::value_type &entry: forward_) {
        if (entry.second.majorNumber() == majr)
            used.push_back(entry.second.minorNumber());
    }
    return firstUnused(used);
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::getLargestRegisters() const {
    SortBySize order(SortBySize::ASCENDING);
    return filterNonoverlapping(getDescriptors(), order, true);
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::getSmallestRegisters() const {
    SortBySize order(SortBySize::DESCENDING);
    return filterNonoverlapping(getDescriptors(), order, true);
}

void
RegisterDictionary::print(std::ostream &o) const {
    o <<"RegisterDictionary \"" <<name_ <<"\" contains " <<forward_.size() <<" " <<(1 == forward_.size()?"entry":"entries") <<"\n";
    for (Entries::const_iterator ri = forward_.begin(); ri != forward_.end(); ++ri)
        o <<"  \"" <<ri->first <<"\" " <<ri->second <<"\n";

    for (Reverse::const_iterator ri = reverse_.begin(); ri != reverse_.end(); ++ri) {
        o <<"  " <<ri->first;
        for (std::vector<std::string>::const_iterator vi = ri->second.begin(); vi != ri->second.end(); ++vi) {
            o <<" " <<*vi;
        }
        o <<"\n";
    }
}

size_t
RegisterDictionary::size() const {
    return forward_.size();
}


Sawyer::Optional<std::string>
RegisterDictionary::name(RegisterDescriptor reg) const {
    const std::string nm = lookup(reg);
    if (nm.empty()) {
        return {};
    } else {
        return nm;
    }
}

std::string
RegisterDictionary::nameOrQuad(RegisterDescriptor reg) const {
    if (const auto s = name(reg)) {
        return *s;
    } else {
        return reg.toString();
    }
}

std::string
RegisterDictionary::nameAndQuad(RegisterDescriptor reg) const {
    const std::string q = reg.toString();
    const auto s = name(reg);
    return s ? "\"" + StringUtility::cEscape(*s) + "\" " + q : q;
}

std::string
RegisterDictionary::quadAndName(RegisterDescriptor reg) const {
    const std::string q = reg.toString();
    const auto s = name(reg);
    return s ? q + " \"" + StringUtility::cEscape(*s) + "\"" : q;
}

std::ostream&
operator<<(std::ostream &out, const RegisterDictionary &dict) {
    dict.print(out);
    return out;
}

RegisterDescriptor
RegisterDictionary::instructionPointerRegister() const {
    return instructionPointer_;
}

void
RegisterDictionary::instructionPointerRegister(RegisterDescriptor reg) {
    instructionPointer_ = reg;
}

void
RegisterDictionary::instructionPointerRegister(const std::string &name) {
    instructionPointer_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::stackPointerRegister() const {
    return stackPointer_;
}

void
RegisterDictionary::stackPointerRegister(RegisterDescriptor reg) {
    stackPointer_ = reg;
}

void
RegisterDictionary::stackPointerRegister(const std::string &name) {
    stackPointer_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::stackFrameRegister() const {
    return stackFrame_;
}

void
RegisterDictionary::stackFrameRegister(RegisterDescriptor reg) {
    stackFrame_ = reg;
}

void
RegisterDictionary::stackFrameRegister(const std::string &name) {
    stackFrame_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::stackSegmentRegister() const {
    return stackSegment_;
}

void
RegisterDictionary::stackSegmentRegister(RegisterDescriptor reg) {
    stackSegment_ = reg;
}

void
RegisterDictionary::stackSegmentRegister(const std::string &name) {
    stackSegment_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::callReturnRegister() const {
    return callReturn_;
}

void
RegisterDictionary::callReturnRegister(RegisterDescriptor reg) {
    callReturn_ = reg;
}

void
RegisterDictionary::callReturnRegister(const std::string &name) {
    callReturn_ = findOrThrow(name);
}

} // namespace
} // namespace

#endif
