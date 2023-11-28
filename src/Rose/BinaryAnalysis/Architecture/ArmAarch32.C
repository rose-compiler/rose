#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch32.h>
#include <Rose/BinaryAnalysis/Unparser/Aarch32.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

ArmAarch32::ArmAarch32(InstructionSet instructionSet)
    : Base(InstructionSet::T32 == instructionSet ? "arm-t32" : "arm-a32", 4, ByteOrder::ORDER_LSB),
      instructionSet_(instructionSet) {}

ArmAarch32::~ArmAarch32() {}

ArmAarch32::Ptr
ArmAarch32::instance(InstructionSet instructionSet) {
    return Ptr(new ArmAarch32(instructionSet));
}

ArmAarch32::InstructionSet
ArmAarch32::instructionSet() const {
    return instructionSet_;
}

RegisterDictionary::Ptr
ArmAarch32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());

        // Pseudo register that returns a new free variable every time it's read.
        regs->insert("unknown", aarch32_regclass_sys, aarch32_sys_unknown, 0, 32);

        // 16 general purpose registers R0-R12, "sp" (stack pointer), "lr" (link register), and "pc" (program counter). The stack
        // pointer and link register are banked; that is, there are more than one stack register and link register but only one is
        // visible at a time depending on the current "system level view".
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

        // banked SPSR "saved program status register". The SPSR stores the value of the CPSR "current program status register" when
        // an exception is taken so that it can be restored after handling the exception.  Each exception handling mode can access
        // its own SPSR. User mode and System mode do not have an SPSR because they are not exception handling states.
        regs->insert("spsr_hyp", aarch32_regclass_sys, aarch32_sys_spsr_hyp, 0, 32);
        regs->insert("spsr_svc", aarch32_regclass_sys, aarch32_sys_spsr_svc, 0, 32);
        regs->insert("spsr_abt", aarch32_regclass_sys, aarch32_sys_spsr_abt, 0, 32);
        regs->insert("spsr_und", aarch32_regclass_sys, aarch32_sys_spsr_und, 0, 32);
        regs->insert("spsr_mon", aarch32_regclass_sys, aarch32_sys_spsr_mon, 0, 32);
        regs->insert("spsr_irq", aarch32_regclass_sys, aarch32_sys_spsr_irq, 0, 32);
        regs->insert("spsr_fiq", aarch32_regclass_sys, aarch32_sys_spsr_fiq, 0, 32);

        // When an instruction is being decoded and the AST is being produced, we don't know what exception handling mode the
        // processor will be in when the instruction is executed. Therefore, ROSE creates a special "spsr" register whose read and
        // write operations will require translation to the correct SPSR hardware register later.
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

        // VFP11 system registers. The VFPv2 architecture describes the following three system registers that must be present in a
        // VFP system.
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

        // To support exceptional conditions, the VFP11 coprocessor provides two additional registers. These registers are designed
        // to be used with the support code software available from ARM Limited. As a result, this documentation [from ARM] does not
        // fully specify exception handling in all cases.
        regs->insert("fpinst",  aarch32_regclass_sys, aarch32_sys_fpinst,  0, 32); // floating-point instruction register
        regs->insert("fpinst2", aarch32_regclass_sys, aarch32_sys_fpinst2, 0, 32); // floatinglcpoint instruction register two

        // The VFP11 coprocessor also provides two feature registers.
        regs->insert("mvfr0",   aarch32_regclass_sys, aarch32_sys_mvfr0, 0, 32); // media and VFP feature register 0
        regs->insert("mvfr1",   aarch32_regclass_sys, aarch32_sys_mvfr1, 0, 32); // media and VFP feature register 1

        // MVFR2, mediao, and VFP feature register 2. Describes the features provided by the AArch32 Advanced SIMD and
        // Floating-point implementation. Must be interpreted with MVFR0 and MVFR1. This register is present only when AArch32 is
        // supported at any exception level. Otherwise, direct accesses to MVFR2 are undefined. Implemented only if the
        // implementation includes Advanced SIMD and floating-point instructions.
        regs->insert("mvfr2",   aarch32_regclass_sys, aarch32_sys_mvfr2, 0, 32);

        // Thumb IT instructions
        regs->insert("itstate", aarch32_regclass_sys, aarch32_sys_itstate, 0, 32);

        // NEON and VFP use the same extension register bank. This is distinct from the ARM register bank. The extension register
        // bank is a colleciton of registers which can be accesed as either 32-bit, 64-bit, or 128-bit registers, depending on
        // whether the instruction is NEON or VFP.
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

        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
ArmAarch32::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family) {
        if (header->get_executableFormat()->get_wordSize() == 4 && InstructionSet::A32 == instructionSet())
            return true;
        if (header->get_executableFormat()->get_wordSize() == 2 && InstructionSet::T32 == instructionSet())
            return true;
    }
    return false;
}

Disassembler::Base::Ptr
ArmAarch32::newInstructionDecoder() const {
    switch (instructionSet()) {
        case InstructionSet::A32:
            return Disassembler::Aarch32::instanceA32(shared_from_this());
        case InstructionSet::T32:
            return Disassembler::Aarch32::instanceT32(shared_from_this());
    }
    ASSERT_not_reachable("invalid instruction set");
}

Unparser::Base::Ptr
ArmAarch32::newUnparser() const {
    return Unparser::Aarch32::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
ArmAarch32::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherAarch32::instance(shared_from_this(), ops);
}

} // namespace
} // namespace
} // namespace

#endif
