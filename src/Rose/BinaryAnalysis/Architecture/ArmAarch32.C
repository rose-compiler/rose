#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherAarch32.h>
#include <Rose/BinaryAnalysis/Unparser/Aarch32.h>

#include <SgAsmAarch32Instruction.h>
#include <SgAsmExecutableFileFormat.h>
#include <SgAsmGenericHeader.h>
#include <SgAsmIntegerValueExpression.h>
#include <Cxx_GrammarDowncast.h>

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

Sawyer::Container::Interval<size_t>
ArmAarch32::bytesPerInstruction() const {
    switch (instructionSet()) {
        case InstructionSet::A32:
            return 2;
        case InstructionSet::T32:
            return 4;
    }
    ASSERT_not_reachable("invalid instruction set");
}

Alignment
ArmAarch32::instructionAlignment() const {
    switch (instructionSet()) {
        case InstructionSet::A32:
            return Alignment(2, bitsPerWord());
        case InstructionSet::T32:
            return Alignment(4, bitsPerWord());
    }
    ASSERT_not_reachable("invalid instruction set");
}

std::string
ArmAarch32::instructionMnemonic(const SgAsmInstruction *insn_) const {
    if (isUnknown(insn_))
        return "unknown";

    using Kind = Aarch32InstructionKind;
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);

    const std::string base = [insn]() {
        switch (insn->get_kind()) {
            case Kind::ARM_INS_ENDING:      break;
            case Kind::ARM_INS_INVALID:     return "unknown";      // not a valid instruction

            case Kind::ARM_INS_ADC:         return "adc";
            case Kind::ARM_INS_ADD:         return "add";
            case Kind::ARM_INS_ADR:         return "adr";
            case Kind::ARM_INS_AESD:        return "aesd";
            case Kind::ARM_INS_AESE:        return "aese";
            case Kind::ARM_INS_AESIMC:      return "aesimc";
            case Kind::ARM_INS_AESMC:       return "aesmc";
            case Kind::ARM_INS_ADDW:        return "addw";
            case Kind::ARM_INS_AND:         return "and";
            case Kind::ARM_INS_ASR:         return "asr"; // alias of MOV, MOVS
            case Kind::ARM_INS_B:           return "b";
            case Kind::ARM_INS_BFC:         return "bfc";
            case Kind::ARM_INS_BFI:         return "bfi";
            case Kind::ARM_INS_BIC:         return "bic";
            case Kind::ARM_INS_BKPT:        return "bkpt";
            case Kind::ARM_INS_BL:          return "bl";
            case Kind::ARM_INS_BLX:         return "blx";
            case Kind::ARM_INS_BXJ:         return "bxj";
            case Kind::ARM_INS_BX:          return "bx";
            case Kind::ARM_INS_CBNZ:        return "cbnz";
            case Kind::ARM_INS_CBZ:         return "cbz";
            case Kind::ARM_INS_CDP:         return "cdp";
            case Kind::ARM_INS_CDP2:        return "cdp2";
            case Kind::ARM_INS_CLREX:       return "clrex";
            case Kind::ARM_INS_CLZ:         return "clz";
            case Kind::ARM_INS_CMN:         return "cmn";
            case Kind::ARM_INS_CMP:         return "cmp";
            case Kind::ARM_INS_CPS:         return "cps";
            case Kind::ARM_INS_CRC32B:      return "crc32b";
            case Kind::ARM_INS_CRC32CB:     return "crc32cb";
            case Kind::ARM_INS_CRC32CH:     return "crc32ch";
            case Kind::ARM_INS_CRC32CW:     return "crc32cw";
            case Kind::ARM_INS_CRC32H:      return "crc32h";
            case Kind::ARM_INS_CRC32W:      return "crc32w";
            case Kind::ARM_INS_DBG:         return "dbg";
            case Kind::ARM_INS_DCPS1:       return "dcps1";
            case Kind::ARM_INS_DCPS2:       return "dcps2";
            case Kind::ARM_INS_DCPS3:       return "dcps3";
            case Kind::ARM_INS_DMB:         return "dmb";
            case Kind::ARM_INS_DSB:         return "dsb";
            case Kind::ARM_INS_EOR:         return "eor";
            case Kind::ARM_INS_ERET:        return "eret";
            case Kind::ARM_INS_FLDMDBX:     return "fldmdbx";
            case Kind::ARM_INS_FLDMIAX:     return "fldmiax";
            case Kind::ARM_INS_FSTMDBX:     return "fstmdbx";
            case Kind::ARM_INS_FSTMIAX:     return "fstmiax";
            case Kind::ARM_INS_HINT:        return "hint";
            case Kind::ARM_INS_HLT:         return "hlt";
            case Kind::ARM_INS_HVC:         return "hvc";
            case Kind::ARM_INS_ISB:         return "isb";
            case Kind::ARM_INS_IT:          return "it";
            case Kind::ARM_INS_LDA:         return "lda";
            case Kind::ARM_INS_LDAB:        return "ldab";
            case Kind::ARM_INS_LDAEX:       return "ldaex";
            case Kind::ARM_INS_LDAEXB:      return "ldaexb";
            case Kind::ARM_INS_LDAEXD:      return "ldaexd";
            case Kind::ARM_INS_LDAEXH:      return "ldaexh";
            case Kind::ARM_INS_LDAH:        return "ldah";
            case Kind::ARM_INS_LDC:         return "ldc";
            case Kind::ARM_INS_LDC2:        return "ldc2"; // version 5 and above
            case Kind::ARM_INS_LDC2L:       return "ldc2l"; // version 5 and above
            case Kind::ARM_INS_LDCL:        return "ldcl";
            case Kind::ARM_INS_LDM:         return "ldm";
            case Kind::ARM_INS_LDMDA:       return "ldmda";
            case Kind::ARM_INS_LDMDB:       return "ldmdb";
            case Kind::ARM_INS_LDMIB:       return "ldmib";
            case Kind::ARM_INS_LDR:         return "ldr";
            case Kind::ARM_INS_LDRB:        return "ldrb";
            case Kind::ARM_INS_LDRBT:       return "ldrbt";
            case Kind::ARM_INS_LDRD:        return "ldrd";
            case Kind::ARM_INS_LDREX:       return "ldrex";
            case Kind::ARM_INS_LDREXB:      return "ldrexb";
            case Kind::ARM_INS_LDREXD:      return "ldrexd";
            case Kind::ARM_INS_LDREXH:      return "ldrexh";
            case Kind::ARM_INS_LDRH:        return "ldrh";
            case Kind::ARM_INS_LDRHT:       return "ldrht";
            case Kind::ARM_INS_LDRSB:       return "ldrsb";
            case Kind::ARM_INS_LDRSBT:      return "ldrsbt";
            case Kind::ARM_INS_LDRSH:       return "ldrsh";
            case Kind::ARM_INS_LDRSHT:      return "ldrsht";
            case Kind::ARM_INS_LDRT:        return "ldrt";
            case Kind::ARM_INS_LSL:         return "lsl"; // alias of MOV, MOVS
            case Kind::ARM_INS_LSR:         return "lsr"; // alias of MOV, MOVS
            case Kind::ARM_INS_MCR:         return "mcr";
            case Kind::ARM_INS_MCR2:        return "mcr2";
            case Kind::ARM_INS_MCRR:        return "mcrr";
            case Kind::ARM_INS_MCRR2:       return "mcrr2";
            case Kind::ARM_INS_MLA:         return "mla";
            case Kind::ARM_INS_MLS:         return "mls";
            case Kind::ARM_INS_MOV:         return "mov";
            case Kind::ARM_INS_MOVT:        return "movt";
            case Kind::ARM_INS_MOVW:        return "movw";
            case Kind::ARM_INS_MRC:         return "mrc";
            case Kind::ARM_INS_MRC2:        return "mrc2";
            case Kind::ARM_INS_MRRC:        return "mrrc";
            case Kind::ARM_INS_MRRC2:       return "mrrc2";
            case Kind::ARM_INS_MRS:         return "mrs";
            case Kind::ARM_INS_MSR:         return "msr";
            case Kind::ARM_INS_MUL:         return "mul";
            case Kind::ARM_INS_MVN:         return "mvn";
            case Kind::ARM_INS_NOP:         return "nop";
            case Kind::ARM_INS_ORN:         return "orn";
            case Kind::ARM_INS_ORR:         return "orr";
            case Kind::ARM_INS_PKHBT:       return "pkhbt";
            case Kind::ARM_INS_PKHTB:       return "pkhtb";
            case Kind::ARM_INS_PLD:         return "pld";
            case Kind::ARM_INS_PLDW:        return "pldw";
            case Kind::ARM_INS_PLI:         return "pli";
            case Kind::ARM_INS_POP:         return "pop";
            case Kind::ARM_INS_PUSH:        return "push";
            case Kind::ARM_INS_QADD:        return "qadd";
            case Kind::ARM_INS_QADD16:      return "qadd16";
            case Kind::ARM_INS_QADD8:       return "qadd8";
            case Kind::ARM_INS_QASX:        return "qasx";
            case Kind::ARM_INS_QDADD:       return "qdadd";
            case Kind::ARM_INS_QDSUB:       return "qdsub";
            case Kind::ARM_INS_QSAX:        return "qsax";
            case Kind::ARM_INS_QSUB:        return "qsub";
            case Kind::ARM_INS_QSUB16:      return "qsub16";
            case Kind::ARM_INS_QSUB8:       return "qsub8";
            case Kind::ARM_INS_RBIT:        return "rbit";
            case Kind::ARM_INS_REV:         return "rev";
            case Kind::ARM_INS_REV16:       return "rev16";
            case Kind::ARM_INS_REVSH:       return "revsh";
            case Kind::ARM_INS_RFEDA:       return "rfeda";
            case Kind::ARM_INS_RFEDB:       return "rfedb";
            case Kind::ARM_INS_RFEIA:       return "rfeia";
            case Kind::ARM_INS_RFEIB:       return "rfeib";
            case Kind::ARM_INS_ROR:         return "ror"; // alias of MOV, MOVS
            case Kind::ARM_INS_RRX:         return "rrx"; // alias of MOV, MOVS
            case Kind::ARM_INS_RSB:         return "rsb";
            case Kind::ARM_INS_RSC:         return "rsc";
            case Kind::ARM_INS_SADD16:      return "sadd16";
            case Kind::ARM_INS_SADD8:       return "sadd8";
            case Kind::ARM_INS_SASX:        return "sasx";
            case Kind::ARM_INS_SBC:         return "sbc";
            case Kind::ARM_INS_SBFX:        return "sbfx";
            case Kind::ARM_INS_SDIV:        return "sdiv";
            case Kind::ARM_INS_SEL:         return "sel";
            case Kind::ARM_INS_SETEND:      return "setend";
            case Kind::ARM_INS_SEV:         return "sev";
            case Kind::ARM_INS_SEVL:        return "sevl";
            case Kind::ARM_INS_SHA1C:       return "sha1c";
            case Kind::ARM_INS_SHA1H:       return "sha1h";
            case Kind::ARM_INS_SHA1M:       return "sha1m";
            case Kind::ARM_INS_SHA1P:       return "sha1p";
            case Kind::ARM_INS_SHA1SU0:     return "sha1su0";
            case Kind::ARM_INS_SHA1SU1:     return "sha1su1";
            case Kind::ARM_INS_SHA256H:     return "sha256h";
            case Kind::ARM_INS_SHA256H2:    return "sha256h2";
            case Kind::ARM_INS_SHA256SU0:   return "sha256su0";
            case Kind::ARM_INS_SHA256SU1:   return "sha256su1";
            case Kind::ARM_INS_SHADD16:     return "shadd16";
            case Kind::ARM_INS_SHADD8:      return "shadd8";
            case Kind::ARM_INS_SHASX:       return "shasx";
            case Kind::ARM_INS_SHSAX:       return "shsax";
            case Kind::ARM_INS_SHSUB16:     return "shsub16";
            case Kind::ARM_INS_SHSUB8:      return "shsub8";
            case Kind::ARM_INS_SMC:         return "smc";
            case Kind::ARM_INS_SMLABB:      return "smlabb";
            case Kind::ARM_INS_SMLABT:      return "smlabt";
            case Kind::ARM_INS_SMLAD:       return "smlad";
            case Kind::ARM_INS_SMLADX:      return "smladx";
            case Kind::ARM_INS_SMLAL:       return "smlal";
            case Kind::ARM_INS_SMLALBB:     return "smlalbb";
            case Kind::ARM_INS_SMLALBT:     return "smlalbt";
            case Kind::ARM_INS_SMLALD:      return "smlald";
            case Kind::ARM_INS_SMLALDX:     return "smlaldx";
            case Kind::ARM_INS_SMLALTB:     return "smlaltb";
            case Kind::ARM_INS_SMLALTT:     return "smlaltt";
            case Kind::ARM_INS_SMLATB:      return "smlatb";
            case Kind::ARM_INS_SMLATT:      return "smlatt";
            case Kind::ARM_INS_SMLAWB:      return "smlawb";
            case Kind::ARM_INS_SMLAWT:      return "smlawt";
            case Kind::ARM_INS_SMLSD:       return "smlsd";
            case Kind::ARM_INS_SMLSDX:      return "smlsdx";
            case Kind::ARM_INS_SMLSLD:      return "smlsld";
            case Kind::ARM_INS_SMLSLDX:     return "smlsldx";
            case Kind::ARM_INS_SMMLA:       return "smmla";
            case Kind::ARM_INS_SMMLAR:      return "smmlar";
            case Kind::ARM_INS_SMMLS:       return "smmls";
            case Kind::ARM_INS_SMMLSR:      return "smmlsr";
            case Kind::ARM_INS_SMMUL:       return "smmul";
            case Kind::ARM_INS_SMMULR:      return "smmulr";
            case Kind::ARM_INS_SMUAD:       return "smuad";
            case Kind::ARM_INS_SMUADX:      return "smuadx";
            case Kind::ARM_INS_SMULBB:      return "smulbb";
            case Kind::ARM_INS_SMULBT:      return "smulbt";
            case Kind::ARM_INS_SMULL:       return "smull";
            case Kind::ARM_INS_SMULTB:      return "smultb";
            case Kind::ARM_INS_SMULTT:      return "smultt";
            case Kind::ARM_INS_SMULWB:      return "smulwb";
            case Kind::ARM_INS_SMULWT:      return "smulwt";
            case Kind::ARM_INS_SMUSD:       return "smusd";
            case Kind::ARM_INS_SMUSDX:      return "smusdx";
            case Kind::ARM_INS_SRSDA:       return "srsda";
            case Kind::ARM_INS_SRSDB:       return "srsdb";
            case Kind::ARM_INS_SRSIA:       return "srsia";
            case Kind::ARM_INS_SRSIB:       return "srsib";
            case Kind::ARM_INS_SSAT:        return "ssat";
            case Kind::ARM_INS_SSAT16:      return "ssat16";
            case Kind::ARM_INS_SSAX:        return "ssax";
            case Kind::ARM_INS_SSUB16:      return "ssub16";
            case Kind::ARM_INS_SSUB8:       return "ssub8";
            case Kind::ARM_INS_STC:         return "stc";
            case Kind::ARM_INS_STC2:        return "stc2"; // ARMv5 and above
            case Kind::ARM_INS_STC2L:       return "stc2l"; // ARMv5 and above
            case Kind::ARM_INS_STCL:        return "stcl";
            case Kind::ARM_INS_STL:         return "stl";
            case Kind::ARM_INS_STLB:        return "stlb";
            case Kind::ARM_INS_STLEX:       return "stlex";
            case Kind::ARM_INS_STLEXB:      return "stlexb";
            case Kind::ARM_INS_STLEXD:      return "stlexd";
            case Kind::ARM_INS_STLEXH:      return "stlexh";
            case Kind::ARM_INS_STLH:        return "stlh";
            case Kind::ARM_INS_STM:         return "stm";
            case Kind::ARM_INS_STMDA:       return "stmda";
            case Kind::ARM_INS_STMDB:       return "stmdb";
            case Kind::ARM_INS_STMIB:       return "stmib";
            case Kind::ARM_INS_STR:         return "str";
            case Kind::ARM_INS_STRB:        return "strb";
            case Kind::ARM_INS_STRBT:       return "strbt";
            case Kind::ARM_INS_STRD:        return "strd";
            case Kind::ARM_INS_STREX:       return "strex";
            case Kind::ARM_INS_STREXB:      return "strexb";
            case Kind::ARM_INS_STREXD:      return "strexd";
            case Kind::ARM_INS_STREXH:      return "strexh";
            case Kind::ARM_INS_STRH:        return "strh";
            case Kind::ARM_INS_STRHT:       return "strht";
            case Kind::ARM_INS_STRT:        return "strt";
            case Kind::ARM_INS_SUB:         return "sub";
            case Kind::ARM_INS_SUBW:        return "subw";
            case Kind::ARM_INS_SVC:         return "svc";
            case Kind::ARM_INS_SWP:         return "swp";
            case Kind::ARM_INS_SWPB:        return "swpb";
            case Kind::ARM_INS_SXTAB:       return "sxtab";
            case Kind::ARM_INS_SXTAB16:     return "sxtab16";
            case Kind::ARM_INS_SXTAH:       return "sxtah";
            case Kind::ARM_INS_SXTB:        return "sxtb";
            case Kind::ARM_INS_SXTB16:      return "sxtb16";
            case Kind::ARM_INS_SXTH:        return "sxth";
            case Kind::ARM_INS_TBB:         return "tbb";
            case Kind::ARM_INS_TBH:         return "tbh";
            case Kind::ARM_INS_TEQ:         return "teq";
            case Kind::ARM_INS_TRAP:        return "trap";
            case Kind::ARM_INS_TST:         return "tst";
            case Kind::ARM_INS_UADD16:      return "uadd16";
            case Kind::ARM_INS_UADD8:       return "uadd8";
            case Kind::ARM_INS_UASX:        return "uasx";
            case Kind::ARM_INS_UBFX:        return "ubfx";
            case Kind::ARM_INS_UDF:         return "udf";
            case Kind::ARM_INS_UDIV:        return "udiv";
            case Kind::ARM_INS_UHADD16:     return "uhadd16";
            case Kind::ARM_INS_UHADD8:      return "uhadd8";
            case Kind::ARM_INS_UHASX:       return "uhasx";
            case Kind::ARM_INS_UHSAX:       return "uhsax";
            case Kind::ARM_INS_UHSUB16:     return "uhsub16";
            case Kind::ARM_INS_UHSUB8:      return "uhsub8";
            case Kind::ARM_INS_UMAAL:       return "umaal";
            case Kind::ARM_INS_UMLAL:       return "umlal";
            case Kind::ARM_INS_UMULL:       return "umull";
            case Kind::ARM_INS_UQADD16:     return "uqadd16";
            case Kind::ARM_INS_UQADD8:      return "uqadd8";
            case Kind::ARM_INS_UQASX:       return "uqasx";
            case Kind::ARM_INS_UQSAX:       return "uqsax";
            case Kind::ARM_INS_UQSUB16:     return "uqsub16";
            case Kind::ARM_INS_UQSUB8:      return "uqsub8";
            case Kind::ARM_INS_USAD8:       return "usad8";
            case Kind::ARM_INS_USADA8:      return "usada8";
            case Kind::ARM_INS_USAT:        return "usat";
            case Kind::ARM_INS_USAT16:      return "usat16";
            case Kind::ARM_INS_USAX:        return "usax";
            case Kind::ARM_INS_USUB16:      return "usub16";
            case Kind::ARM_INS_USUB8:       return "usub8";
            case Kind::ARM_INS_UXTAB:       return "uxtab";
            case Kind::ARM_INS_UXTAB16:     return "uxtab16";
            case Kind::ARM_INS_UXTAH:       return "uxtah";
            case Kind::ARM_INS_UXTB:        return "uxtb";
            case Kind::ARM_INS_UXTB16:      return "uxtb16";
            case Kind::ARM_INS_UXTH:        return "uxth";
            case Kind::ARM_INS_VABA:        return "vaba";
            case Kind::ARM_INS_VABAL:       return "vabal";
            case Kind::ARM_INS_VABD:        return "vabd";
            case Kind::ARM_INS_VABDL:       return "vabdl";
            case Kind::ARM_INS_VABS:        return "vabs";
            case Kind::ARM_INS_VACGE:       return "vacge";
            case Kind::ARM_INS_VACGT:       return "vacgt";
            case Kind::ARM_INS_VADD:        return "vadd";
            case Kind::ARM_INS_VADDHN:      return "vaddhn";
            case Kind::ARM_INS_VADDL:       return "vaddl";
            case Kind::ARM_INS_VADDW:       return "vaddw";
            case Kind::ARM_INS_VAND:        return "vand";
            case Kind::ARM_INS_VBIC:        return "vbic";
            case Kind::ARM_INS_VBIF:        return "vbif";
            case Kind::ARM_INS_VBIT:        return "vbit";
            case Kind::ARM_INS_VBSL:        return "vbsl";
            case Kind::ARM_INS_VCEQ:        return "vceq";
            case Kind::ARM_INS_VCGE:        return "vcge";
            case Kind::ARM_INS_VCGT:        return "vcgt";
            case Kind::ARM_INS_VCLE:        return "vcle";
            case Kind::ARM_INS_VCLS:        return "vcls";
            case Kind::ARM_INS_VCLT:        return "vclt";
            case Kind::ARM_INS_VCLZ:        return "vclz";
            case Kind::ARM_INS_VCMP:        return "vcmp";
            case Kind::ARM_INS_VCMPE:       return "vcmpe";
            case Kind::ARM_INS_VCNT:        return "vcnt";
            case Kind::ARM_INS_VCVT:        return "vcvt";
            case Kind::ARM_INS_VCVTA:       return "vcvta";
            case Kind::ARM_INS_VCVTB:       return "vcvtb";
            case Kind::ARM_INS_VCVTM:       return "vcvtm";
            case Kind::ARM_INS_VCVTN:       return "vcvtn";
            case Kind::ARM_INS_VCVTP:       return "vcvtp";
            case Kind::ARM_INS_VCVTR:       return "vcvtr";
            case Kind::ARM_INS_VCVTT:       return "vcvtt";
            case Kind::ARM_INS_VDIV:        return "vdiv";
            case Kind::ARM_INS_VDUP:        return "vdup";
            case Kind::ARM_INS_VEOR:        return "veor";
            case Kind::ARM_INS_VEXT:        return "vext";
            case Kind::ARM_INS_VFMA:        return "vfma";
            case Kind::ARM_INS_VFMS:        return "vfms";
            case Kind::ARM_INS_VFNMA:       return "vfnma";
            case Kind::ARM_INS_VFNMS:       return "vfnms";
            case Kind::ARM_INS_VHADD:       return "vhadd";
            case Kind::ARM_INS_VHSUB:       return "vhsub";
            case Kind::ARM_INS_VLD1:        return "vld1";
            case Kind::ARM_INS_VLD2:        return "vld2";
            case Kind::ARM_INS_VLD3:        return "vld3";
            case Kind::ARM_INS_VLD4:        return "vld4";
            case Kind::ARM_INS_VLDMDB:      return "vldmdb";
            case Kind::ARM_INS_VLDMIA:      return "vldmia";
            case Kind::ARM_INS_VLDR:        return "vldr";
            case Kind::ARM_INS_VMAX:        return "vmax";
            case Kind::ARM_INS_VMAXNM:      return "vmaxnm";
            case Kind::ARM_INS_VMIN:        return "vmin";
            case Kind::ARM_INS_VMINNM:      return "vminnm";
            case Kind::ARM_INS_VMLA:        return "vmla";
            case Kind::ARM_INS_VMLAL:       return "vmlal";
            case Kind::ARM_INS_VMLS:        return "vmls";
            case Kind::ARM_INS_VMLSL:       return "vmlsl";
            case Kind::ARM_INS_VMOV:        return "vmov";
            case Kind::ARM_INS_VMOVL:       return "vmovl";
            case Kind::ARM_INS_VMOVN:       return "vmovn";
            case Kind::ARM_INS_VMRS:        return "vmrs";
            case Kind::ARM_INS_VMSR:        return "vmsr";
            case Kind::ARM_INS_VMUL:        return "vmul";
            case Kind::ARM_INS_VMULL:       return "vmull";
            case Kind::ARM_INS_VMVN:        return "vmvn";
            case Kind::ARM_INS_VNEG:        return "vneg";
            case Kind::ARM_INS_VNMLA:       return "vnmla";
            case Kind::ARM_INS_VNMLS:       return "vnmls";
            case Kind::ARM_INS_VNMUL:       return "vnmul";
            case Kind::ARM_INS_VORN:        return "vorn";
            case Kind::ARM_INS_VORR:        return "vorr";
            case Kind::ARM_INS_VPADAL:      return "vpadal";
            case Kind::ARM_INS_VPADD:       return "vpadd";
            case Kind::ARM_INS_VPADDL:      return "vpaddl";
            case Kind::ARM_INS_VPMAX:       return "vpmax";
            case Kind::ARM_INS_VPMIN:       return "vpmin";
            case Kind::ARM_INS_VPOP:        return "vpop"; // aliases VLDM*
            case Kind::ARM_INS_VPUSH:       return "vpush"; // aliases STMDB, VSTM*
            case Kind::ARM_INS_VQABS:       return "vqabs";
            case Kind::ARM_INS_VQADD:       return "vqadd";
            case Kind::ARM_INS_VQDMLAL:     return "vqdmlal";
            case Kind::ARM_INS_VQDMLSL:     return "vqdmlsl";
            case Kind::ARM_INS_VQDMULH:     return "vqdmulh";
            case Kind::ARM_INS_VQDMULL:     return "vqdmull";
            case Kind::ARM_INS_VQMOVN:      return "vqmovn";
            case Kind::ARM_INS_VQMOVUN:     return "vqmovun";
            case Kind::ARM_INS_VQNEG:       return "vqneg";
            case Kind::ARM_INS_VQRDMULH:    return "vqrdmulh";
            case Kind::ARM_INS_VQRSHL:      return "vqrshl";
            case Kind::ARM_INS_VQRSHRN:     return "vqrshrn";
            case Kind::ARM_INS_VQRSHRUN:    return "vqrshrun";
            case Kind::ARM_INS_VQSHL:       return "vqshl";
            case Kind::ARM_INS_VQSHLU:      return "vqshlu";
            case Kind::ARM_INS_VQSHRN:      return "vqshrn";
            case Kind::ARM_INS_VQSHRUN:     return "vqshrun";
            case Kind::ARM_INS_VQSUB:       return "vqsub";
            case Kind::ARM_INS_VRADDHN:     return "vraddhn";
            case Kind::ARM_INS_VRECPE:      return "vrecpe";
            case Kind::ARM_INS_VRECPS:      return "vrecps";
            case Kind::ARM_INS_VREV16:      return "vrev16";
            case Kind::ARM_INS_VREV32:      return "vrev32";
            case Kind::ARM_INS_VREV64:      return "vrev64";
            case Kind::ARM_INS_VRHADD:      return "vrhadd";
            case Kind::ARM_INS_VRINTA:      return "vrinta";
            case Kind::ARM_INS_VRINTM:      return "vrintm";
            case Kind::ARM_INS_VRINTN:      return "vrintn";
            case Kind::ARM_INS_VRINTP:      return "vrintp";
            case Kind::ARM_INS_VRINTR:      return "vrintr";
            case Kind::ARM_INS_VRINTX:      return "vrintx";
            case Kind::ARM_INS_VRINTZ:      return "vrintz";
            case Kind::ARM_INS_VRSHL:       return "vrshl";
            case Kind::ARM_INS_VRSHR:       return "vrshr";
            case Kind::ARM_INS_VRSHRN:      return "vrshrn";
            case Kind::ARM_INS_VRSQRTE:     return "vrsqrte";
            case Kind::ARM_INS_VRSQRTS:     return "vrsqrts";
            case Kind::ARM_INS_VRSRA:       return "vrsra";
            case Kind::ARM_INS_VRSUBHN:     return "vrsubhn";
            case Kind::ARM_INS_VSELEQ:      return "vseleq";
            case Kind::ARM_INS_VSELGE:      return "vselge";
            case Kind::ARM_INS_VSELGT:      return "vselgt";
            case Kind::ARM_INS_VSELVS:      return "vselvs";
            case Kind::ARM_INS_VSHL:        return "vshl";
            case Kind::ARM_INS_VSHLL:       return "vshll";
            case Kind::ARM_INS_VSHR:        return "vshr";
            case Kind::ARM_INS_VSHRN:       return "vshrn";
            case Kind::ARM_INS_VSLI:        return "vsli";
            case Kind::ARM_INS_VSQRT:       return "vsqrt";
            case Kind::ARM_INS_VSRA:        return "vsra";
            case Kind::ARM_INS_VSRI:        return "vsri";
            case Kind::ARM_INS_VST1:        return "vst1";
            case Kind::ARM_INS_VST2:        return "vst2";
            case Kind::ARM_INS_VST3:        return "vst3";
            case Kind::ARM_INS_VST4:        return "vst4";
            case Kind::ARM_INS_VSTMDB:      return "vstmdb";
            case Kind::ARM_INS_VSTMIA:      return "vstmia";
            case Kind::ARM_INS_VSTR:        return "vstr";
            case Kind::ARM_INS_VSUB:        return "vsub";
            case Kind::ARM_INS_VSUBHN:      return "vsubhn";
            case Kind::ARM_INS_VSUBL:       return "vsubl";
            case Kind::ARM_INS_VSUBW:       return "vsubw";
            case Kind::ARM_INS_VSWP:        return "vswp";
            case Kind::ARM_INS_VTBL:        return "vtbl";
            case Kind::ARM_INS_VTBX:        return "vtbx";
            case Kind::ARM_INS_VTRN:        return "vtrn";
            case Kind::ARM_INS_VTST:        return "vtst";
            case Kind::ARM_INS_VUZP:        return "vuzp";
            case Kind::ARM_INS_VZIP:        return "vzip";
            case Kind::ARM_INS_WFE:         return "wfe";
            case Kind::ARM_INS_WFI:         return "wfi";
            case Kind::ARM_INS_YIELD:       return "yield";
        }
        ASSERT_not_reachable("invalid AArch32 instruction kind: " + StringUtility::numberToString(insn->get_kind()));
    }();

    const std::string suffix = [insn]() {
        switch (insn->get_condition()) {
            case ARM_CC_INVALID:        break;
            case ARM_CC_EQ:             return ".eq";
            case ARM_CC_NE:             return ".ne";
            case ARM_CC_HS:             return ".hs";
            case ARM_CC_LO:             return ".lo";
            case ARM_CC_MI:             return ".mi";
            case ARM_CC_PL:             return ".pl";
            case ARM_CC_VS:             return ".vs";
            case ARM_CC_VC:             return ".vc";
            case ARM_CC_HI:             return ".hi";
            case ARM_CC_LS:             return ".ls";
            case ARM_CC_GE:             return ".ge";
            case ARM_CC_LT:             return ".lt";
            case ARM_CC_GT:             return ".gt";
            case ARM_CC_LE:             return ".le";
            case ARM_CC_AL:             return "";
        }
        ASSERT_not_reachable("invalid AAarch32 condition: " + StringUtility::numberToString(insn->get_condition()));
    }();

    return base + (insn->get_updatesFlags() ? "s" : "") + suffix;
}

std::string
ArmAarch32::instructionDescription(const SgAsmInstruction *insn_) const {
    using Kind = Aarch32InstructionKind;
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case Kind::ARM_INS_ENDING:      return "";      // not a valid instruction
        case Kind::ARM_INS_INVALID:     return "";      // not a valid instruction
        case Kind::ARM_INS_ADC:         return "add with carry";
        case Kind::ARM_INS_ADD:         return "add";
        case Kind::ARM_INS_ADR:         return "form PC-relative address";
        case Kind::ARM_INS_AESD:        return "AES single round encryption";
        case Kind::ARM_INS_AESE:        return "AES single round encryption";
        case Kind::ARM_INS_AESIMC:      return "AES inverse mix columns";
        case Kind::ARM_INS_AESMC:       return "AES mix columns";
        case Kind::ARM_INS_ADDW:        return "add";
        case Kind::ARM_INS_AND:         return "bitwise AND";
        case Kind::ARM_INS_ASR:         return "arithmetic shift right"; // alias of MOV, MOVS
        case Kind::ARM_INS_B:           return "branch";
        case Kind::ARM_INS_BFC:         return "bitfield clear";
        case Kind::ARM_INS_BFI:         return "bitfield insert";
        case Kind::ARM_INS_BIC:         return "bitwise bit clear";
        case Kind::ARM_INS_BKPT:        return "breakpoint";
        case Kind::ARM_INS_BL:          return "branch with link";
        case Kind::ARM_INS_BLX:         return "branch with link and exchange";
        case Kind::ARM_INS_BXJ:         return "branch and exchange Jazelle";
        case Kind::ARM_INS_BX:          return "branch and exchange";
        case Kind::ARM_INS_CBNZ:        return "compare and branch on nonzero";
        case Kind::ARM_INS_CBZ:         return "compare and branch on zero";
        case Kind::ARM_INS_CDP:         return "coprocessor data operation";
        case Kind::ARM_INS_CDP2:        return "coprocessor data operation";
        case Kind::ARM_INS_CLREX:       return "clear exclusive";
        case Kind::ARM_INS_CLZ:         return "count leading zeros";
        case Kind::ARM_INS_CMN:         return "compare negative";
        case Kind::ARM_INS_CMP:         return "compare";
        case Kind::ARM_INS_CPS:         return "change PE state";
        case Kind::ARM_INS_CRC32B:      return "perform cyclic redundancy check";
        case Kind::ARM_INS_CRC32CB:     return "perform cyclic redundancy check";
        case Kind::ARM_INS_CRC32CH:     return "perform cyclic redundancy check";
        case Kind::ARM_INS_CRC32CW:     return "perform cyclic redundancy check";
        case Kind::ARM_INS_CRC32H:      return "perform cyclic redundancy check";
        case Kind::ARM_INS_CRC32W:      return "perform cyclic redundancy check";
        case Kind::ARM_INS_DBG:         return "no operation";
        case Kind::ARM_INS_DCPS1:       return "debug change PE state to EL1";
        case Kind::ARM_INS_DCPS2:       return "debug change PE state to EL2";
        case Kind::ARM_INS_DCPS3:       return "debug change PE state to EL3";
        case Kind::ARM_INS_DMB:         return "data memory barrier";
        case Kind::ARM_INS_DSB:         return "data synchronization barrier";
        case Kind::ARM_INS_EOR:         return "bitwise exclusive OR";
        case Kind::ARM_INS_ERET:        return "exception return";
        case Kind::ARM_INS_FLDMDBX:     return "load multiple SIMD&FP decrement before";
        case Kind::ARM_INS_FLDMIAX:     return "load multiple SIMD&FP increment after";
        case Kind::ARM_INS_FSTMDBX:     return "store multiple SIMD&FP decrement before";
        case Kind::ARM_INS_FSTMIAX:     return "store multiple SIMD&FP increment after";
        case Kind::ARM_INS_HINT:        return "hint";
        case Kind::ARM_INS_HLT:         return "halting breakpoint";
        case Kind::ARM_INS_HVC:         return "hypervisor call";
        case Kind::ARM_INS_ISB:         return "instruction synchronization barrier";
        case Kind::ARM_INS_IT:          return "if-then";
        case Kind::ARM_INS_LDA:         return "load-acquire word";
        case Kind::ARM_INS_LDAB:        return "load-acquire byte";
        case Kind::ARM_INS_LDAEX:       return "load-acquire exclusive word";
        case Kind::ARM_INS_LDAEXB:      return "load-acquire exclusive bytes";
        case Kind::ARM_INS_LDAEXD:      return "load-acquire exclusive double-word";
        case Kind::ARM_INS_LDAEXH:      return "load-acquire exclusive half-word";
        case Kind::ARM_INS_LDAH:        return "load-acquire half-word";
        case Kind::ARM_INS_LDC:         return "load data to coprocessor";
        case Kind::ARM_INS_LDC2:        return "load data to coprocessor"; // version 5 and above
        case Kind::ARM_INS_LDC2L:       return "load data to coprocessor long"; // version 5 and above
        case Kind::ARM_INS_LDCL:        return "load data to coprocessor long";
        case Kind::ARM_INS_LDM:         return "load multiple";
        case Kind::ARM_INS_LDMDA:       return "load multiple decrement after";
        case Kind::ARM_INS_LDMDB:       return "load mulitple decrement before";
        case Kind::ARM_INS_LDMIB:       return "load multiple increment before";
        case Kind::ARM_INS_LDR:         return "load register";
        case Kind::ARM_INS_LDRB:        return "load register byte";
        case Kind::ARM_INS_LDRBT:       return "load register byte unprivileged";
        case Kind::ARM_INS_LDRD:        return "load register dual";
        case Kind::ARM_INS_LDREX:       return "load register exclusive";
        case Kind::ARM_INS_LDREXB:      return "load register exclusive byte";
        case Kind::ARM_INS_LDREXD:      return "load register exclusive double-word";
        case Kind::ARM_INS_LDREXH:      return "load register exclusive half-word";
        case Kind::ARM_INS_LDRH:        return "load register half-word";
        case Kind::ARM_INS_LDRHT:       return "load register half-word unprivileged";
        case Kind::ARM_INS_LDRSB:       return "load register signed byte";
        case Kind::ARM_INS_LDRSBT:      return "load register signed byte unprivileged";
        case Kind::ARM_INS_LDRSH:       return "load register signed half-word";
        case Kind::ARM_INS_LDRSHT:      return "load register signed half-word unprivileged";
        case Kind::ARM_INS_LDRT:        return "load register unprivileged";
        case Kind::ARM_INS_LSL:         return "logical shift left"; // alias of MOV, MOVS
        case Kind::ARM_INS_LSR:         return "logical shift right"; // alias of MOV, MOVS
        case Kind::ARM_INS_MCR:         return "move general-purpose register to coprocessor";
        case Kind::ARM_INS_MCR2:        return "move general-purpose register to coprocessor";
        case Kind::ARM_INS_MCRR:        return "move two general-purpose registers to coprocessor";
        case Kind::ARM_INS_MCRR2:       return "move two general-purpose registers to coprocessor";
        case Kind::ARM_INS_MLA:         return "multiply accumulate";
        case Kind::ARM_INS_MLS:         return "multiply and subtract";
        case Kind::ARM_INS_MOV:         return "move";
        case Kind::ARM_INS_MOVT:        return "move top";
        case Kind::ARM_INS_MOVW:        return "move";
        case Kind::ARM_INS_MRC:         return "move coprocessor to general-purpose register";
        case Kind::ARM_INS_MRC2:        return "move coprocessor to general-purpose register";
        case Kind::ARM_INS_MRRC:        return "move coprocessor to two general-purpose registers";
        case Kind::ARM_INS_MRRC2:       return "move coprocessor to two general-purpose registers";
        case Kind::ARM_INS_MRS:         return "move special register to general-purpose register";
        case Kind::ARM_INS_MSR:         return "move to special register";
        case Kind::ARM_INS_MUL:         return "multiply";
        case Kind::ARM_INS_MVN:         return "bitwise NOT";
        case Kind::ARM_INS_NOP:         return "no operation";
        case Kind::ARM_INS_ORN:         return "bitwise OR NOT";
        case Kind::ARM_INS_ORR:         return "bitwise OR";
        case Kind::ARM_INS_PKHBT:       return "pack half-word";
        case Kind::ARM_INS_PKHTB:       return "pack half-word";
        case Kind::ARM_INS_PLD:         return "preload data";
        case Kind::ARM_INS_PLDW:        return "preload data";
        case Kind::ARM_INS_PLI:         return "preload instruction";
        case Kind::ARM_INS_POP:         return "pop multiple registers from stack";
        case Kind::ARM_INS_PUSH:        return "push multiple registers to stack";
        case Kind::ARM_INS_QADD:        return "saturating add";
        case Kind::ARM_INS_QADD16:      return "saturating add 16";
        case Kind::ARM_INS_QADD8:       return "saturating add 8";
        case Kind::ARM_INS_QASX:        return "saturating add and exchange";
        case Kind::ARM_INS_QDADD:       return "saturating double and add";
        case Kind::ARM_INS_QDSUB:       return "saturating double and subtract";
        case Kind::ARM_INS_QSAX:        return "saturating subtract and add with exchange";
        case Kind::ARM_INS_QSUB:        return "saturating subtract";
        case Kind::ARM_INS_QSUB16:      return "saturating subtract 16";
        case Kind::ARM_INS_QSUB8:       return "saturating subtract 8";
        case Kind::ARM_INS_RBIT:        return "reverse bits";
        case Kind::ARM_INS_REV:         return "byte-reverse word";
        case Kind::ARM_INS_REV16:       return "byte-reverse packed half-word";
        case Kind::ARM_INS_REVSH:       return "byte-reverse signed half-word";
        case Kind::ARM_INS_RFEDA:       return "return from exception";
        case Kind::ARM_INS_RFEDB:       return "return from exception";
        case Kind::ARM_INS_RFEIA:       return "return from exception";
        case Kind::ARM_INS_RFEIB:       return "return from exception";
        case Kind::ARM_INS_ROR:         return "rotate right"; // alias of MOV, MOVS
        case Kind::ARM_INS_RRX:         return "rotate right with extend"; // alias of MOV, MOVS
        case Kind::ARM_INS_RSB:         return "reverse subtract";
        case Kind::ARM_INS_RSC:         return "reverse subtract with carry";
        case Kind::ARM_INS_SADD16:      return "signed add 16";
        case Kind::ARM_INS_SADD8:       return "signed add 8";
        case Kind::ARM_INS_SASX:        return "signed add and subtract with exchange";
        case Kind::ARM_INS_SBC:         return "subtract with carry";
        case Kind::ARM_INS_SBFX:        return "signed bitfield extract";
        case Kind::ARM_INS_SDIV:        return "signed divide";
        case Kind::ARM_INS_SEL:         return "select bytes";
        case Kind::ARM_INS_SETEND:      return "set endianness";
        case Kind::ARM_INS_SEV:         return "send event";
        case Kind::ARM_INS_SEVL:        return "send event local";
        case Kind::ARM_INS_SHA1C:       return "SHA1 hash update choose";
        case Kind::ARM_INS_SHA1H:       return "SHA1 fixed rotate";
        case Kind::ARM_INS_SHA1M:       return "SHA1 hash update majority";
        case Kind::ARM_INS_SHA1P:       return "SHA1 hash update parity";
        case Kind::ARM_INS_SHA1SU0:     return "SHA1 schedule update 0";
        case Kind::ARM_INS_SHA1SU1:     return "SHA1 schedule udpate 1";
        case Kind::ARM_INS_SHA256H:     return "SHA256 hash update part 1";
        case Kind::ARM_INS_SHA256H2:    return "SHA256 hash update part 2";
        case Kind::ARM_INS_SHA256SU0:   return "SHA256 schedule update 0";
        case Kind::ARM_INS_SHA256SU1:   return "SHA256 schedule update 1";
        case Kind::ARM_INS_SHADD16:     return "signed halving add 16";
        case Kind::ARM_INS_SHADD8:      return "signed halving add 8";
        case Kind::ARM_INS_SHASX:       return "signed halving add and subtract with exchange";
        case Kind::ARM_INS_SHSAX:       return "signed halving subtract and add with exchange";
        case Kind::ARM_INS_SHSUB16:     return "signed halving subtract 16";
        case Kind::ARM_INS_SHSUB8:      return "signed halving subtract 8";
        case Kind::ARM_INS_SMC:         return "secure monitor call";
        case Kind::ARM_INS_SMLABB:      return "signed multiply accumulate (half-words)";
        case Kind::ARM_INS_SMLABT:      return "signed multiply accumulate (lalf-words)";
        case Kind::ARM_INS_SMLAD:       return "signed multiply accumulate dual";
        case Kind::ARM_INS_SMLADX:      return "signed multiply accumulate dual";
        case Kind::ARM_INS_SMLAL:       return "signed multiply accumulate long";
        case Kind::ARM_INS_SMLALBB:     return "signed multiply accumulate long (half-words)";
        case Kind::ARM_INS_SMLALBT:     return "signed multiply accumulate long (half-words)";
        case Kind::ARM_INS_SMLALD:      return "signed multiply accumulate long dual";
        case Kind::ARM_INS_SMLALDX:     return "signed multiply accumulate long dual";
        case Kind::ARM_INS_SMLALTB:     return "signed multiply accumulate (half-words)";
        case Kind::ARM_INS_SMLALTT:     return "signed multiply accumulate (half-words)";
        case Kind::ARM_INS_SMLATB:      return "signed multiply accumulate long (half-words)";
        case Kind::ARM_INS_SMLATT:      return "signed multiply accumulate (half-words)";
        case Kind::ARM_INS_SMLAWB:      return "signed multiply accumulate (word by half-word)";
        case Kind::ARM_INS_SMLAWT:      return "signed multiply accumulate (word by half-word)";
        case Kind::ARM_INS_SMLSD:       return "signed multiply subtract dual";
        case Kind::ARM_INS_SMLSDX:      return "signed multiply subtract dual";
        case Kind::ARM_INS_SMLSLD:      return "signed multiply subtract long dual";
        case Kind::ARM_INS_SMLSLDX:     return "signed multiply subtract long dual";
        case Kind::ARM_INS_SMMLA:       return "signed most significant word multiply accumulate";
        case Kind::ARM_INS_SMMLAR:      return "signed most significant word multiply accumulate";
        case Kind::ARM_INS_SMMLS:       return "signed most significant word multiply subtract";
        case Kind::ARM_INS_SMMLSR:      return "signed most significant word multiply subtract";
        case Kind::ARM_INS_SMMUL:       return "signed most significant word multiply";
        case Kind::ARM_INS_SMMULR:      return "signed most significant word multiply";
        case Kind::ARM_INS_SMUAD:       return "signed dual multiply add";
        case Kind::ARM_INS_SMUADX:      return "signed dual multiply add";
        case Kind::ARM_INS_SMULBB:      return "signed multiply (half-words)";
        case Kind::ARM_INS_SMULBT:      return "signed multiply (half-words)";
        case Kind::ARM_INS_SMULL:       return "signed multiply long";
        case Kind::ARM_INS_SMULTB:      return "signed multiply (half-words)";
        case Kind::ARM_INS_SMULTT:      return "signed multiply (half-words)";
        case Kind::ARM_INS_SMULWB:      return "signed multiply (word by half-word)";
        case Kind::ARM_INS_SMULWT:      return "signed multiply (word by half-word)";
        case Kind::ARM_INS_SMUSD:       return "signed multiply subtract dual";
        case Kind::ARM_INS_SMUSDX:      return "signed multiply subtract dual";
        case Kind::ARM_INS_SRSDA:       return "store return state";
        case Kind::ARM_INS_SRSDB:       return "store return state";
        case Kind::ARM_INS_SRSIA:       return "store return state";
        case Kind::ARM_INS_SRSIB:       return "store return state";
        case Kind::ARM_INS_SSAT:        return "signed saturate";
        case Kind::ARM_INS_SSAT16:      return "signed saturate 16";
        case Kind::ARM_INS_SSAX:        return "signed subtract and add with exchange";
        case Kind::ARM_INS_SSUB16:      return "signed subtract 16";
        case Kind::ARM_INS_SSUB8:       return "signed subtract 8";
        case Kind::ARM_INS_STC:         return "store coprocessor";
        case Kind::ARM_INS_STC2:        return "store coprocessor"; // ARMv5 and above
        case Kind::ARM_INS_STC2L:       return "store coprocessor long"; // ARMv5 and above
        case Kind::ARM_INS_STCL:        return "store coprocessor long";
        case Kind::ARM_INS_STL:         return "store release word";
        case Kind::ARM_INS_STLB:        return "store release byte";
        case Kind::ARM_INS_STLEX:       return "store release exclusive word";
        case Kind::ARM_INS_STLEXB:      return "store release exclusive byte";
        case Kind::ARM_INS_STLEXD:      return "store release exclusive double-word";
        case Kind::ARM_INS_STLEXH:      return "store release exclusive half-word";
        case Kind::ARM_INS_STLH:        return "store release half-word";
        case Kind::ARM_INS_STM:         return "store multiple";
        case Kind::ARM_INS_STMDA:       return "store multiple decrement after";
        case Kind::ARM_INS_STMDB:       return "store multiple decrement before";
        case Kind::ARM_INS_STMIB:       return "store multiple increment before";
        case Kind::ARM_INS_STR:         return "store register";
        case Kind::ARM_INS_STRB:        return "store register byte";
        case Kind::ARM_INS_STRBT:       return "store register byte unprivileged";
        case Kind::ARM_INS_STRD:        return "store register dual";
        case Kind::ARM_INS_STREX:       return "store register exclusive";
        case Kind::ARM_INS_STREXB:      return "store register exclusive byte";
        case Kind::ARM_INS_STREXD:      return "store register exclusive double-word";
        case Kind::ARM_INS_STREXH:      return "store register exclusive half-word";
        case Kind::ARM_INS_STRH:        return "store register half-word";
        case Kind::ARM_INS_STRHT:       return "store register half-word unprivileged";
        case Kind::ARM_INS_STRT:        return "store register unprivileged";
        case Kind::ARM_INS_SUB:         return "subtract";
        case Kind::ARM_INS_SUBW:        return "subtract";
        case Kind::ARM_INS_SVC:         return "supervisor call";
        case Kind::ARM_INS_SWP:         return "swap between registers and memory";
        case Kind::ARM_INS_SWPB:        return "swap byte between registers and memory";
        case Kind::ARM_INS_SXTAB:       return "sign extend and add byte";
        case Kind::ARM_INS_SXTAB16:     return "sign extend and add byte 16";
        case Kind::ARM_INS_SXTAH:       return "sign extend and add half-word";
        case Kind::ARM_INS_SXTB:        return "sign extend byte";
        case Kind::ARM_INS_SXTB16:      return "sign extend byte 16";
        case Kind::ARM_INS_SXTH:        return "sign extend half-word";
        case Kind::ARM_INS_TBB:         return "table branch byte";
        case Kind::ARM_INS_TBH:         return "table branch half-word";
        case Kind::ARM_INS_TEQ:         return "test equivalence";
        case Kind::ARM_INS_TRAP:        return "trap";
        case Kind::ARM_INS_TST:         return "test";
        case Kind::ARM_INS_UADD16:      return "unsigned add 16";
        case Kind::ARM_INS_UADD8:       return "unsigned add 8";
        case Kind::ARM_INS_UASX:        return "unsigned add and subtract with exchange";
        case Kind::ARM_INS_UBFX:        return "unsigned bitfield extract";
        case Kind::ARM_INS_UDF:         return "permanently undefined";
        case Kind::ARM_INS_UDIV:        return "unsigned divide";
        case Kind::ARM_INS_UHADD16:     return "unsigned halving add 16";
        case Kind::ARM_INS_UHADD8:      return "unsigned halving add 8";
        case Kind::ARM_INS_UHASX:       return "unsigned halving add and subtract with exchange";
        case Kind::ARM_INS_UHSAX:       return "unsigned halving subtract and add with exchange";
        case Kind::ARM_INS_UHSUB16:     return "unsigned halving subtract 16";
        case Kind::ARM_INS_UHSUB8:      return "unsigned halving subtract 8";
        case Kind::ARM_INS_UMAAL:       return "unsigned multiply accumulate accumulate long";
        case Kind::ARM_INS_UMLAL:       return "unsigned multiply accumulate long";
        case Kind::ARM_INS_UMULL:       return "unsigned multiply long";
        case Kind::ARM_INS_UQADD16:     return "unsigned saturating add 16";
        case Kind::ARM_INS_UQADD8:      return "unsigned saturating add 8";
        case Kind::ARM_INS_UQASX:       return "unsigned saturating add and subtract with exchange";
        case Kind::ARM_INS_UQSAX:       return "unsigned saturating subtract and add with exchange";
        case Kind::ARM_INS_UQSUB16:     return "unsigned saturating subtract 16";
        case Kind::ARM_INS_UQSUB8:      return "unsigned saturating subtract 8";
        case Kind::ARM_INS_USAD8:       return "unsigned sum of absolute differences";
        case Kind::ARM_INS_USADA8:      return "unsigned sum of absolute differences 8";
        case Kind::ARM_INS_USAT:        return "unsigned saturate";
        case Kind::ARM_INS_USAT16:      return "unsigned saturate 16";
        case Kind::ARM_INS_USAX:        return "unsigned subtract and add with exchange";
        case Kind::ARM_INS_USUB16:      return "unsigned subtract 16";
        case Kind::ARM_INS_USUB8:       return "unsigned subtract 8";
        case Kind::ARM_INS_UXTAB:       return "unsigned extend and add byte";
        case Kind::ARM_INS_UXTAB16:     return "unsigned extend and add byte 16";
        case Kind::ARM_INS_UXTAH:       return "unsigned extend and add half-word";
        case Kind::ARM_INS_UXTB:        return "unsigned extend byte";
        case Kind::ARM_INS_UXTB16:      return "unsigned extend byte 16";
        case Kind::ARM_INS_UXTH:        return "unsigned extend half-word";
        case Kind::ARM_INS_VABA:        return "vector absolute difference and accumulate";
        case Kind::ARM_INS_VABAL:       return "vector absolute difference and accumulate long";
        case Kind::ARM_INS_VABD:        return "vector absolute difference";
        case Kind::ARM_INS_VABDL:       return "vector absolute difference long";
        case Kind::ARM_INS_VABS:        return "vector absolute";
        case Kind::ARM_INS_VACGE:       return "vector absolute compare greater than or equal";
        case Kind::ARM_INS_VACGT:       return "vector absolute compare greater than";
        case Kind::ARM_INS_VADD:        return "vector add";
        case Kind::ARM_INS_VADDHN:      return "vector add and narrow returning high half";
        case Kind::ARM_INS_VADDL:       return "vector add long";
        case Kind::ARM_INS_VADDW:       return "vector add wide";
        case Kind::ARM_INS_VAND:        return "vector bitwise AND";
        case Kind::ARM_INS_VBIC:        return "vector bitwise bit clear";
        case Kind::ARM_INS_VBIF:        return "vector bitwise insert if false";
        case Kind::ARM_INS_VBIT:        return "vector bitwise insert if true";
        case Kind::ARM_INS_VBSL:        return "vector bitwise select";
        case Kind::ARM_INS_VCEQ:        return "vector compare equal to zero";
        case Kind::ARM_INS_VCGE:        return "vector compare greater than or equal to zero";
        case Kind::ARM_INS_VCGT:        return "vector compare greater than zero";
        case Kind::ARM_INS_VCLE:        return "vector compare less than or equal to zero";
        case Kind::ARM_INS_VCLS:        return "vector count leading sign bits";
        case Kind::ARM_INS_VCLT:        return "vector compare less than zero";
        case Kind::ARM_INS_VCLZ:        return "vector count leading zeros";
        case Kind::ARM_INS_VCMP:        return "vector compare";
        case Kind::ARM_INS_VCMPE:       return "vector compare rasing invalid operation on NaN";
        case Kind::ARM_INS_VCNT:        return "vector count set bits";
        case Kind::ARM_INS_VCVT:        return "vector convert";
        case Kind::ARM_INS_VCVTA:       return "vector convert with round";
        case Kind::ARM_INS_VCVTB:       return "vector convert to/from half-precision in bottom half of single-precision";
        case Kind::ARM_INS_VCVTM:       return "vector convert round towards negative infinity";
        case Kind::ARM_INS_VCVTN:       return "vector convert round towards nearest";
        case Kind::ARM_INS_VCVTP:       return "vector convert round towards positive infinity";
        case Kind::ARM_INS_VCVTR:       return "vector convert FP to integer";
        case Kind::ARM_INS_VCVTT:       return "vector convert to/from half-precision in top half of single-precision";
        case Kind::ARM_INS_VDIV:        return "vector divide";
        case Kind::ARM_INS_VDUP:        return "vector duplicate";
        case Kind::ARM_INS_VEOR:        return "vector bitwise exclusive OR";
        case Kind::ARM_INS_VEXT:        return "vector extract";
        case Kind::ARM_INS_VFMA:        return "vector fused multiply accumulate";
        case Kind::ARM_INS_VFMS:        return "vector fused multiply subtract";
        case Kind::ARM_INS_VFNMA:       return "vector fused negate multiply accumulate";
        case Kind::ARM_INS_VFNMS:       return "vector fused negate multiply subtract";
        case Kind::ARM_INS_VHADD:       return "vector halving add";
        case Kind::ARM_INS_VHSUB:       return "vector halving subtract";
        case Kind::ARM_INS_VLD1:        return "load one-element structure";
        case Kind::ARM_INS_VLD2:        return "load two-element structure";
        case Kind::ARM_INS_VLD3:        return "load three-element structure";
        case Kind::ARM_INS_VLD4:        return "load four-element structure";
        case Kind::ARM_INS_VLDMDB:      return "load multiple SIMD&FP decrement before";
        case Kind::ARM_INS_VLDMIA:      return "load multiple SIMD&FP increment after";
        case Kind::ARM_INS_VLDR:        return "load SIMD&FP register";
        case Kind::ARM_INS_VMAX:        return "vector maximum";
        case Kind::ARM_INS_VMAXNM:      return "vector maximum NaN";
        case Kind::ARM_INS_VMIN:        return "vector minimum";
        case Kind::ARM_INS_VMINNM:      return "vector minimum NaN";
        case Kind::ARM_INS_VMLA:        return "vector multiply accumulate";
        case Kind::ARM_INS_VMLAL:       return "vector multiply accumulate long";
        case Kind::ARM_INS_VMLS:        return "vector multiply subtract";
        case Kind::ARM_INS_VMLSL:       return "vector multiply subtract long";
        case Kind::ARM_INS_VMOV:        return "vector move";
        case Kind::ARM_INS_VMOVL:       return "vector move long";
        case Kind::ARM_INS_VMOVN:       return "vector move and narrow";
        case Kind::ARM_INS_VMRS:        return "move SIMD&FP special register to general-purpose register";
        case Kind::ARM_INS_VMSR:        return "move general-purpose register to SIMD&FP special register";
        case Kind::ARM_INS_VMUL:        return "vector multiply";
        case Kind::ARM_INS_VMULL:       return "vector multiply long";
        case Kind::ARM_INS_VMVN:        return "vector bitwise NOT";
        case Kind::ARM_INS_VNEG:        return "vector negate";
        case Kind::ARM_INS_VNMLA:       return "vector negate multiply accumulate";
        case Kind::ARM_INS_VNMLS:       return "vector negate multiply subtract";
        case Kind::ARM_INS_VNMUL:       return "vector negate multiply";
        case Kind::ARM_INS_VORN:        return "vector bitwise OR NOT";
        case Kind::ARM_INS_VORR:        return "vector bitwise OR";
        case Kind::ARM_INS_VPADAL:      return "vector pairwise add and accumulate long";
        case Kind::ARM_INS_VPADD:       return "vector pairwise add";
        case Kind::ARM_INS_VPADDL:      return "vector pairwise add long";
        case Kind::ARM_INS_VPMAX:       return "vector pairwise maximum";
        case Kind::ARM_INS_VPMIN:       return "vector pairwise minimum";
        case Kind::ARM_INS_VPOP:        return "pop extension registers from stack"; // aliases VLDM*
        case Kind::ARM_INS_VPUSH:       return "push registers onto a full descending stack"; // aliases STMDB, VSTM*
        case Kind::ARM_INS_VQABS:       return "vector saturating absolute";
        case Kind::ARM_INS_VQADD:       return "vector saturating add";
        case Kind::ARM_INS_VQDMLAL:     return "vector saturating doubling multiply accumulate long";
        case Kind::ARM_INS_VQDMLSL:     return "vector saturating doubling multiply subtract long";
        case Kind::ARM_INS_VQDMULH:     return "vector saturating doubling multiply returning high half";
        case Kind::ARM_INS_VQDMULL:     return "vector saturating doubling multiply long";
        case Kind::ARM_INS_VQMOVN:      return "vector saturating move and narrow";
        case Kind::ARM_INS_VQMOVUN:     return "vector saturating move and narrow unsigned";
        case Kind::ARM_INS_VQNEG:       return "vector saturating negate";
        case Kind::ARM_INS_VQRDMULH:    return "vector saturating rounding doubling multiply returning high half";
        case Kind::ARM_INS_VQRSHL:      return "vector saturating rounding shift left";
        case Kind::ARM_INS_VQRSHRN:     return "vector saturating rounding shift left narrow";
        case Kind::ARM_INS_VQRSHRUN:    return "vector saturating rounding shift left narrow unsigned";
        case Kind::ARM_INS_VQSHL:       return "vector saturating shift left";
        case Kind::ARM_INS_VQSHLU:      return "vector saturating shift left unsigned";
        case Kind::ARM_INS_VQSHRN:      return "vector saturating shift right narrow";
        case Kind::ARM_INS_VQSHRUN:     return "vector saturating shift right narrow unsigned";
        case Kind::ARM_INS_VQSUB:       return "vector saturating subtract";
        case Kind::ARM_INS_VRADDHN:     return "vector rounding add and narrow returning high half";
        case Kind::ARM_INS_VRECPE:      return "vector reciprocal estimate";
        case Kind::ARM_INS_VRECPS:      return "vector reciprocal step";
        case Kind::ARM_INS_VREV16:      return "vector reverse in half-words";
        case Kind::ARM_INS_VREV32:      return "vector reverse in words";
        case Kind::ARM_INS_VREV64:      return "vector reverse in double-words";
        case Kind::ARM_INS_VRHADD:      return "vector rounding halving add";
        case Kind::ARM_INS_VRINTA:      return "vector round FP to integer towards nearest with ties to away";
        case Kind::ARM_INS_VRINTM:      return "vector round FP to integer towards negative infinity";
        case Kind::ARM_INS_VRINTN:      return "vector round FP to integer to nearest";
        case Kind::ARM_INS_VRINTP:      return "vector round FP to integer towards positive infinity";
        case Kind::ARM_INS_VRINTR:      return "vector round FP to integer";
        case Kind::ARM_INS_VRINTX:      return "vector round FP to integer inexact";
        case Kind::ARM_INS_VRINTZ:      return "vector round FP to integer towards zero";
        case Kind::ARM_INS_VRSHL:       return "vector rounding shift left";
        case Kind::ARM_INS_VRSHR:       return "vector rounding shift right";
        case Kind::ARM_INS_VRSHRN:      return "vector rounding shift right and narrow";
        case Kind::ARM_INS_VRSQRTE:     return "vector reciprocal square root estimate";
        case Kind::ARM_INS_VRSQRTS:     return "vector reciprocal square root step";
        case Kind::ARM_INS_VRSRA:       return "vector rounding shift right and accumulate";
        case Kind::ARM_INS_VRSUBHN:     return "vector rounding subtract and narrow.";
        case Kind::ARM_INS_VSELEQ:      return "FP conditional less than or equal";
        case Kind::ARM_INS_VSELGE:      return "FP conditional greater than or equal";
        case Kind::ARM_INS_VSELGT:      return "FP conditional greater than";
        case Kind::ARM_INS_VSELVS:      return "FP conditional VS";
        case Kind::ARM_INS_VSHL:        return "vector shift left";
        case Kind::ARM_INS_VSHLL:       return "vector shift left long";
        case Kind::ARM_INS_VSHR:        return "vector shift right";
        case Kind::ARM_INS_VSHRN:       return "vector shift right narrow";
        case Kind::ARM_INS_VSLI:        return "vector shift left and insert";
        case Kind::ARM_INS_VSQRT:       return "square root";
        case Kind::ARM_INS_VSRA:        return "vector shift right and accumulate";
        case Kind::ARM_INS_VSRI:        return "vector shift right and insert";
        case Kind::ARM_INS_VST1:        return "store 1-element structure";
        case Kind::ARM_INS_VST2:        return "store 2-element structure";
        case Kind::ARM_INS_VST3:        return "store 3-element structure";
        case Kind::ARM_INS_VST4:        return "store 4-element structure";
        case Kind::ARM_INS_VSTMDB:      return "store multiple SIMD&FP registers decrement before";
        case Kind::ARM_INS_VSTMIA:      return "store multiple SIMD&FP registers increment after";
        case Kind::ARM_INS_VSTR:        return "store SIMD&FP register";
        case Kind::ARM_INS_VSUB:        return "vector subtract";
        case Kind::ARM_INS_VSUBHN:      return "vector subtract and narrow returning high half";
        case Kind::ARM_INS_VSUBL:       return "vector subtract long";
        case Kind::ARM_INS_VSUBW:       return "vector subtract wide";
        case Kind::ARM_INS_VSWP:        return "vector swap";
        case Kind::ARM_INS_VTBL:        return "vector table lookup";
        case Kind::ARM_INS_VTBX:        return "vector table lookup";
        case Kind::ARM_INS_VTRN:        return "vector transpose";
        case Kind::ARM_INS_VTST:        return "vector test bits";
        case Kind::ARM_INS_VUZP:        return "vector unzip";
        case Kind::ARM_INS_VZIP:        return "vector zip";
        case Kind::ARM_INS_WFE:         return "wait for event";
        case Kind::ARM_INS_WFI:         return "wait for interrupt";
        case Kind::ARM_INS_YIELD:       return "yield hint";
    }
    ASSERT_not_reachable("invalid AArch32 instruction kind: " + StringUtility::numberToString(insn->get_kind()));
}

bool
ArmAarch32::isUnknown(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    return Aarch32InstructionKind::ARM_INS_INVALID == insn->get_kind();
}

bool
ArmAarch32::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    return insn->get_writesToIp();
}

bool
ArmAarch32::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t */*target*/,
                               rose_addr_t */*return_va*/) const {
    if (insns.empty())
        return false;
    auto back = isSgAsmAarch32Instruction(insns.back());
    ASSERT_not_null(back);

    switch (back->get_kind()) {
        case Aarch32InstructionKind::ARM_INS_BL:
        case Aarch32InstructionKind::ARM_INS_BLX:
            return true;
        default:
            return false;
    }
}

bool
ArmAarch32::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) const {
    // Function returns are typically performed by popping the return address from the stack.
    if (insns.empty())
        return false;
    auto last = isSgAsmAarch32Instruction(insns.back());
    ASSERT_not_null(last);
    return last->get_kind() == Aarch32InstructionKind::ARM_INS_POP && last->get_writesToIp();
}

Sawyer::Optional<rose_addr_t>
ArmAarch32::branchTarget(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);

    switch (insn->get_kind()) {
        case Aarch32InstructionKind::ARM_INS_INVALID:
        case Aarch32InstructionKind::ARM_INS_ENDING:
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Fall through only.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_AESD:
        case Aarch32InstructionKind::ARM_INS_AESE:
        case Aarch32InstructionKind::ARM_INS_AESIMC:
        case Aarch32InstructionKind::ARM_INS_AESMC:
        case Aarch32InstructionKind::ARM_INS_CLREX:
        case Aarch32InstructionKind::ARM_INS_CMN:
        case Aarch32InstructionKind::ARM_INS_CMP:
        case Aarch32InstructionKind::ARM_INS_CPS:
        case Aarch32InstructionKind::ARM_INS_DBG:
        case Aarch32InstructionKind::ARM_INS_DCPS1:
        case Aarch32InstructionKind::ARM_INS_DCPS2:
        case Aarch32InstructionKind::ARM_INS_DCPS3:
        case Aarch32InstructionKind::ARM_INS_DMB:
        case Aarch32InstructionKind::ARM_INS_DSB:
        case Aarch32InstructionKind::ARM_INS_ISB:
        case Aarch32InstructionKind::ARM_INS_LDC:
        case Aarch32InstructionKind::ARM_INS_MCR:
        case Aarch32InstructionKind::ARM_INS_MCRR:
        case Aarch32InstructionKind::ARM_INS_MSR:
        case Aarch32InstructionKind::ARM_INS_NOP:
        case Aarch32InstructionKind::ARM_INS_PLD:
        case Aarch32InstructionKind::ARM_INS_PLDW:
        case Aarch32InstructionKind::ARM_INS_PLI:
        case Aarch32InstructionKind::ARM_INS_PUSH:
        case Aarch32InstructionKind::ARM_INS_SETEND:
        case Aarch32InstructionKind::ARM_INS_SEV:
        case Aarch32InstructionKind::ARM_INS_SEVL:
        case Aarch32InstructionKind::ARM_INS_SRSDA:
        case Aarch32InstructionKind::ARM_INS_SRSDB:
        case Aarch32InstructionKind::ARM_INS_SRSIA:
        case Aarch32InstructionKind::ARM_INS_SRSIB:
        case Aarch32InstructionKind::ARM_INS_TEQ:
        case Aarch32InstructionKind::ARM_INS_TST:
        case Aarch32InstructionKind::ARM_INS_YIELD:
        case Aarch32InstructionKind::ARM_INS_VABA:
        case Aarch32InstructionKind::ARM_INS_VABAL:
        case Aarch32InstructionKind::ARM_INS_VABD:
        case Aarch32InstructionKind::ARM_INS_VABDL:
        case Aarch32InstructionKind::ARM_INS_VABS:
        case Aarch32InstructionKind::ARM_INS_VACGE:
        case Aarch32InstructionKind::ARM_INS_VACGT:
        case Aarch32InstructionKind::ARM_INS_VADD:
        case Aarch32InstructionKind::ARM_INS_VADDHN:
        case Aarch32InstructionKind::ARM_INS_VADDL:
        case Aarch32InstructionKind::ARM_INS_VADDW:
        case Aarch32InstructionKind::ARM_INS_VAND:
        case Aarch32InstructionKind::ARM_INS_VBIC:
        case Aarch32InstructionKind::ARM_INS_VBIF:
        case Aarch32InstructionKind::ARM_INS_VBIT:
        case Aarch32InstructionKind::ARM_INS_VBSL:
        case Aarch32InstructionKind::ARM_INS_VCEQ:
        case Aarch32InstructionKind::ARM_INS_VCGE:
        case Aarch32InstructionKind::ARM_INS_VCGT:
        case Aarch32InstructionKind::ARM_INS_VCLE:
        case Aarch32InstructionKind::ARM_INS_VCLS:
        case Aarch32InstructionKind::ARM_INS_VCLT:
        case Aarch32InstructionKind::ARM_INS_VCLZ:
        case Aarch32InstructionKind::ARM_INS_VCMP:
        case Aarch32InstructionKind::ARM_INS_VCMPE:
        case Aarch32InstructionKind::ARM_INS_VCNT:
        case Aarch32InstructionKind::ARM_INS_VCVT:
        case Aarch32InstructionKind::ARM_INS_VCVTA:
        case Aarch32InstructionKind::ARM_INS_VCVTB:
        case Aarch32InstructionKind::ARM_INS_VCVTM:
        case Aarch32InstructionKind::ARM_INS_VCVTN:
        case Aarch32InstructionKind::ARM_INS_VCVTP:
        case Aarch32InstructionKind::ARM_INS_VCVTR:
        case Aarch32InstructionKind::ARM_INS_VCVTT:
        case Aarch32InstructionKind::ARM_INS_VDIV:
        case Aarch32InstructionKind::ARM_INS_VDUP:
        case Aarch32InstructionKind::ARM_INS_VEOR:
        case Aarch32InstructionKind::ARM_INS_VEXT:
        case Aarch32InstructionKind::ARM_INS_VFMA:
        case Aarch32InstructionKind::ARM_INS_VFMS:
        case Aarch32InstructionKind::ARM_INS_VFNMA:
        case Aarch32InstructionKind::ARM_INS_VFNMS:
        case Aarch32InstructionKind::ARM_INS_VHADD:
        case Aarch32InstructionKind::ARM_INS_VHSUB:
        case Aarch32InstructionKind::ARM_INS_VLDR:
        case Aarch32InstructionKind::ARM_INS_VMAX:
        case Aarch32InstructionKind::ARM_INS_VMAXNM:
        case Aarch32InstructionKind::ARM_INS_VMIN:
        case Aarch32InstructionKind::ARM_INS_VMINNM:
        case Aarch32InstructionKind::ARM_INS_VMLA:
        case Aarch32InstructionKind::ARM_INS_VMLAL:
        case Aarch32InstructionKind::ARM_INS_VMLS:
        case Aarch32InstructionKind::ARM_INS_VMLSL:
        case Aarch32InstructionKind::ARM_INS_VMOVL:
        case Aarch32InstructionKind::ARM_INS_VMOVN:
        case Aarch32InstructionKind::ARM_INS_VMUL:
        case Aarch32InstructionKind::ARM_INS_VMULL:
        case Aarch32InstructionKind::ARM_INS_VMVN:
        case Aarch32InstructionKind::ARM_INS_VNEG:
        case Aarch32InstructionKind::ARM_INS_VNMLA:
        case Aarch32InstructionKind::ARM_INS_VNMLS:
        case Aarch32InstructionKind::ARM_INS_VNMUL:
        case Aarch32InstructionKind::ARM_INS_VORN:
        case Aarch32InstructionKind::ARM_INS_VORR:
        case Aarch32InstructionKind::ARM_INS_VPADAL:
        case Aarch32InstructionKind::ARM_INS_VPADD:
        case Aarch32InstructionKind::ARM_INS_VPADDL:
        case Aarch32InstructionKind::ARM_INS_VPMAX:
        case Aarch32InstructionKind::ARM_INS_VPMIN:
        case Aarch32InstructionKind::ARM_INS_VQABS:
        case Aarch32InstructionKind::ARM_INS_VQADD:
        case Aarch32InstructionKind::ARM_INS_VQDMLAL:
        case Aarch32InstructionKind::ARM_INS_VQDMLSL:
        case Aarch32InstructionKind::ARM_INS_VQDMULH:
        case Aarch32InstructionKind::ARM_INS_VQDMULL:
        case Aarch32InstructionKind::ARM_INS_VQMOVN:
        case Aarch32InstructionKind::ARM_INS_VQMOVUN:
        case Aarch32InstructionKind::ARM_INS_VQNEG:
        case Aarch32InstructionKind::ARM_INS_VQRDMULH:
        case Aarch32InstructionKind::ARM_INS_VQRSHL:
        case Aarch32InstructionKind::ARM_INS_VQRSHRN:
        case Aarch32InstructionKind::ARM_INS_VQRSHRUN:
        case Aarch32InstructionKind::ARM_INS_VQSHL:
        case Aarch32InstructionKind::ARM_INS_VQSHLU:
        case Aarch32InstructionKind::ARM_INS_VQSHRN:
        case Aarch32InstructionKind::ARM_INS_VQSHRUN:
        case Aarch32InstructionKind::ARM_INS_VQSUB:
        case Aarch32InstructionKind::ARM_INS_VRADDHN:
        case Aarch32InstructionKind::ARM_INS_VRECPE:
        case Aarch32InstructionKind::ARM_INS_VRECPS:
        case Aarch32InstructionKind::ARM_INS_VREV16:
        case Aarch32InstructionKind::ARM_INS_VREV32:
        case Aarch32InstructionKind::ARM_INS_VREV64:
        case Aarch32InstructionKind::ARM_INS_VRHADD:
        case Aarch32InstructionKind::ARM_INS_VRINTA:
        case Aarch32InstructionKind::ARM_INS_VRINTM:
        case Aarch32InstructionKind::ARM_INS_VRINTN:
        case Aarch32InstructionKind::ARM_INS_VRINTP:
        case Aarch32InstructionKind::ARM_INS_VRINTR:
        case Aarch32InstructionKind::ARM_INS_VRINTX:
        case Aarch32InstructionKind::ARM_INS_VRINTZ:
        case Aarch32InstructionKind::ARM_INS_VRSHL:
        case Aarch32InstructionKind::ARM_INS_VRSHR:
        case Aarch32InstructionKind::ARM_INS_VRSHRN:
        case Aarch32InstructionKind::ARM_INS_VRSQRTE:
        case Aarch32InstructionKind::ARM_INS_VRSQRTS:
        case Aarch32InstructionKind::ARM_INS_VRSRA:
        case Aarch32InstructionKind::ARM_INS_VRSUBHN:
        case Aarch32InstructionKind::ARM_INS_VSELEQ:
        case Aarch32InstructionKind::ARM_INS_VSELGE:
        case Aarch32InstructionKind::ARM_INS_VSELGT:
        case Aarch32InstructionKind::ARM_INS_VSELVS:
        case Aarch32InstructionKind::ARM_INS_VSHL:
        case Aarch32InstructionKind::ARM_INS_VSHLL:
        case Aarch32InstructionKind::ARM_INS_VSHR:
        case Aarch32InstructionKind::ARM_INS_VSHRN:
        case Aarch32InstructionKind::ARM_INS_VSLI:
        case Aarch32InstructionKind::ARM_INS_VSQRT:
        case Aarch32InstructionKind::ARM_INS_VSRA:
        case Aarch32InstructionKind::ARM_INS_VSRI:
        case Aarch32InstructionKind::ARM_INS_VSUB:
        case Aarch32InstructionKind::ARM_INS_VSUBHN:
        case Aarch32InstructionKind::ARM_INS_VSUBL:
        case Aarch32InstructionKind::ARM_INS_VSUBW:
        case Aarch32InstructionKind::ARM_INS_VSWP:
        case Aarch32InstructionKind::ARM_INS_VTBL:
        case Aarch32InstructionKind::ARM_INS_VTBX:
        case Aarch32InstructionKind::ARM_INS_VTRN:
        case Aarch32InstructionKind::ARM_INS_VTST:
        case Aarch32InstructionKind::ARM_INS_VUZP:
        case Aarch32InstructionKind::ARM_INS_VZIP:
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Fall through, or UNPREDICTABLE behavior.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_ADC:
        case Aarch32InstructionKind::ARM_INS_BFC:
        case Aarch32InstructionKind::ARM_INS_BFI:
        case Aarch32InstructionKind::ARM_INS_CLZ:
        case Aarch32InstructionKind::ARM_INS_CRC32B:
        case Aarch32InstructionKind::ARM_INS_CRC32H:
        case Aarch32InstructionKind::ARM_INS_CRC32W:
        case Aarch32InstructionKind::ARM_INS_CRC32CB:
        case Aarch32InstructionKind::ARM_INS_CRC32CH:
        case Aarch32InstructionKind::ARM_INS_CRC32CW:
        case Aarch32InstructionKind::ARM_INS_FLDMDBX:
        case Aarch32InstructionKind::ARM_INS_FLDMIAX:
        case Aarch32InstructionKind::ARM_INS_FSTMDBX:
        case Aarch32InstructionKind::ARM_INS_FSTMIAX:
        case Aarch32InstructionKind::ARM_INS_LDA:
        case Aarch32InstructionKind::ARM_INS_LDAB:
        case Aarch32InstructionKind::ARM_INS_LDAEX:
        case Aarch32InstructionKind::ARM_INS_LDAEXB:
        case Aarch32InstructionKind::ARM_INS_LDAEXD:
        case Aarch32InstructionKind::ARM_INS_LDAEXH:
        case Aarch32InstructionKind::ARM_INS_LDAH:
        case Aarch32InstructionKind::ARM_INS_LDRBT:
        case Aarch32InstructionKind::ARM_INS_LDRD:
        case Aarch32InstructionKind::ARM_INS_LDREX:
        case Aarch32InstructionKind::ARM_INS_LDREXB:
        case Aarch32InstructionKind::ARM_INS_LDREXD:
        case Aarch32InstructionKind::ARM_INS_LDREXH:
        case Aarch32InstructionKind::ARM_INS_LDRH:
        case Aarch32InstructionKind::ARM_INS_LDRHT:
        case Aarch32InstructionKind::ARM_INS_LDRSB:
        case Aarch32InstructionKind::ARM_INS_LDRSBT:
        case Aarch32InstructionKind::ARM_INS_LDRSH:
        case Aarch32InstructionKind::ARM_INS_LDRSHT:
        case Aarch32InstructionKind::ARM_INS_LDRT:
        case Aarch32InstructionKind::ARM_INS_MLA:
        case Aarch32InstructionKind::ARM_INS_MLS:
        case Aarch32InstructionKind::ARM_INS_MOVT:
        case Aarch32InstructionKind::ARM_INS_MRRC:
        case Aarch32InstructionKind::ARM_INS_MRS:
        case Aarch32InstructionKind::ARM_INS_MUL:
        case Aarch32InstructionKind::ARM_INS_ORN:
        case Aarch32InstructionKind::ARM_INS_PKHBT:
        case Aarch32InstructionKind::ARM_INS_PKHTB:
        case Aarch32InstructionKind::ARM_INS_QADD:
        case Aarch32InstructionKind::ARM_INS_QADD16:
        case Aarch32InstructionKind::ARM_INS_QADD8:
        case Aarch32InstructionKind::ARM_INS_QASX:
        case Aarch32InstructionKind::ARM_INS_QDADD:
        case Aarch32InstructionKind::ARM_INS_QDSUB:
        case Aarch32InstructionKind::ARM_INS_QSAX:
        case Aarch32InstructionKind::ARM_INS_QSUB:
        case Aarch32InstructionKind::ARM_INS_QSUB16:
        case Aarch32InstructionKind::ARM_INS_QSUB8:
        case Aarch32InstructionKind::ARM_INS_RBIT:
        case Aarch32InstructionKind::ARM_INS_REV:
        case Aarch32InstructionKind::ARM_INS_REV16:
        case Aarch32InstructionKind::ARM_INS_REVSH:
        case Aarch32InstructionKind::ARM_INS_SADD16:
        case Aarch32InstructionKind::ARM_INS_SADD8:
        case Aarch32InstructionKind::ARM_INS_SASX:
        case Aarch32InstructionKind::ARM_INS_SBFX:
        case Aarch32InstructionKind::ARM_INS_SDIV:
        case Aarch32InstructionKind::ARM_INS_SEL:
        case Aarch32InstructionKind::ARM_INS_SHA1C:
        case Aarch32InstructionKind::ARM_INS_SHA1H:
        case Aarch32InstructionKind::ARM_INS_SHA1M:
        case Aarch32InstructionKind::ARM_INS_SHA1P:
        case Aarch32InstructionKind::ARM_INS_SHA1SU0:
        case Aarch32InstructionKind::ARM_INS_SHA1SU1:
        case Aarch32InstructionKind::ARM_INS_SHA256H:
        case Aarch32InstructionKind::ARM_INS_SHA256H2:
        case Aarch32InstructionKind::ARM_INS_SHA256SU0:
        case Aarch32InstructionKind::ARM_INS_SHA256SU1:
        case Aarch32InstructionKind::ARM_INS_SHADD16:
        case Aarch32InstructionKind::ARM_INS_SHADD8:
        case Aarch32InstructionKind::ARM_INS_SHASX:
        case Aarch32InstructionKind::ARM_INS_SHSAX:
        case Aarch32InstructionKind::ARM_INS_SHSUB16:
        case Aarch32InstructionKind::ARM_INS_SHSUB8:
        case Aarch32InstructionKind::ARM_INS_SMLABB:
        case Aarch32InstructionKind::ARM_INS_SMLABT:
        case Aarch32InstructionKind::ARM_INS_SMLATB:
        case Aarch32InstructionKind::ARM_INS_SMLATT:
        case Aarch32InstructionKind::ARM_INS_SMLAD:
        case Aarch32InstructionKind::ARM_INS_SMLADX:
        case Aarch32InstructionKind::ARM_INS_SMLAL:
        case Aarch32InstructionKind::ARM_INS_SMLALBB:
        case Aarch32InstructionKind::ARM_INS_SMLALBT:
        case Aarch32InstructionKind::ARM_INS_SMLALTB:
        case Aarch32InstructionKind::ARM_INS_SMLALTT:
        case Aarch32InstructionKind::ARM_INS_SMLALD:
        case Aarch32InstructionKind::ARM_INS_SMLALDX:
        case Aarch32InstructionKind::ARM_INS_SMLAWB:
        case Aarch32InstructionKind::ARM_INS_SMLAWT:
        case Aarch32InstructionKind::ARM_INS_SMLSD:
        case Aarch32InstructionKind::ARM_INS_SMLSDX:
        case Aarch32InstructionKind::ARM_INS_SMLSLD:
        case Aarch32InstructionKind::ARM_INS_SMLSLDX:
        case Aarch32InstructionKind::ARM_INS_SMMLA:
        case Aarch32InstructionKind::ARM_INS_SMMLAR:
        case Aarch32InstructionKind::ARM_INS_SMMLS:
        case Aarch32InstructionKind::ARM_INS_SMMLSR:
        case Aarch32InstructionKind::ARM_INS_SMMUL:
        case Aarch32InstructionKind::ARM_INS_SMMULR:
        case Aarch32InstructionKind::ARM_INS_SMUAD:
        case Aarch32InstructionKind::ARM_INS_SMUADX:
        case Aarch32InstructionKind::ARM_INS_SMULBB:
        case Aarch32InstructionKind::ARM_INS_SMULBT:
        case Aarch32InstructionKind::ARM_INS_SMULTB:
        case Aarch32InstructionKind::ARM_INS_SMULTT:
        case Aarch32InstructionKind::ARM_INS_SMULL:
        case Aarch32InstructionKind::ARM_INS_SMULWB:
        case Aarch32InstructionKind::ARM_INS_SMULWT:
        case Aarch32InstructionKind::ARM_INS_SMUSD:
        case Aarch32InstructionKind::ARM_INS_SMUSDX:
        case Aarch32InstructionKind::ARM_INS_SSAT:
        case Aarch32InstructionKind::ARM_INS_SSAT16:
        case Aarch32InstructionKind::ARM_INS_SSAX:
        case Aarch32InstructionKind::ARM_INS_SSUB16:
        case Aarch32InstructionKind::ARM_INS_SSUB8:
        case Aarch32InstructionKind::ARM_INS_STL:
        case Aarch32InstructionKind::ARM_INS_STLB:
        case Aarch32InstructionKind::ARM_INS_STLEX:
        case Aarch32InstructionKind::ARM_INS_STLEXB:
        case Aarch32InstructionKind::ARM_INS_STLEXD:
        case Aarch32InstructionKind::ARM_INS_STLEXH:
        case Aarch32InstructionKind::ARM_INS_STLH:
        case Aarch32InstructionKind::ARM_INS_STM:
        case Aarch32InstructionKind::ARM_INS_STMDA:
        case Aarch32InstructionKind::ARM_INS_STMDB:
        case Aarch32InstructionKind::ARM_INS_STMIB:
        case Aarch32InstructionKind::ARM_INS_STR:
        case Aarch32InstructionKind::ARM_INS_STRB:
        case Aarch32InstructionKind::ARM_INS_STRBT:
        case Aarch32InstructionKind::ARM_INS_STRD:
        case Aarch32InstructionKind::ARM_INS_STREX:
        case Aarch32InstructionKind::ARM_INS_STREXB:
        case Aarch32InstructionKind::ARM_INS_STREXD:
        case Aarch32InstructionKind::ARM_INS_STREXH:
        case Aarch32InstructionKind::ARM_INS_STRH:
        case Aarch32InstructionKind::ARM_INS_STRHT:
        case Aarch32InstructionKind::ARM_INS_STRT:
        case Aarch32InstructionKind::ARM_INS_SXTAB:
        case Aarch32InstructionKind::ARM_INS_SXTAB16:
        case Aarch32InstructionKind::ARM_INS_SXTAH:
        case Aarch32InstructionKind::ARM_INS_SXTB:
        case Aarch32InstructionKind::ARM_INS_SXTB16:
        case Aarch32InstructionKind::ARM_INS_SXTH:
        case Aarch32InstructionKind::ARM_INS_UADD16:
        case Aarch32InstructionKind::ARM_INS_UADD8:
        case Aarch32InstructionKind::ARM_INS_UASX:
        case Aarch32InstructionKind::ARM_INS_UBFX:
        case Aarch32InstructionKind::ARM_INS_UDIV:
        case Aarch32InstructionKind::ARM_INS_UHADD16:
        case Aarch32InstructionKind::ARM_INS_UHADD8:
        case Aarch32InstructionKind::ARM_INS_UHASX:
        case Aarch32InstructionKind::ARM_INS_UHSAX:
        case Aarch32InstructionKind::ARM_INS_UHSUB16:
        case Aarch32InstructionKind::ARM_INS_UHSUB8:
        case Aarch32InstructionKind::ARM_INS_UMAAL:
        case Aarch32InstructionKind::ARM_INS_UMLAL:
        case Aarch32InstructionKind::ARM_INS_UMULL:
        case Aarch32InstructionKind::ARM_INS_UQADD16:
        case Aarch32InstructionKind::ARM_INS_UQADD8:
        case Aarch32InstructionKind::ARM_INS_UQASX:
        case Aarch32InstructionKind::ARM_INS_UQSAX:
        case Aarch32InstructionKind::ARM_INS_UQSUB16:
        case Aarch32InstructionKind::ARM_INS_UQSUB8:
        case Aarch32InstructionKind::ARM_INS_USAD8:
        case Aarch32InstructionKind::ARM_INS_USADA8:
        case Aarch32InstructionKind::ARM_INS_USAT:
        case Aarch32InstructionKind::ARM_INS_USAT16:
        case Aarch32InstructionKind::ARM_INS_USAX:
        case Aarch32InstructionKind::ARM_INS_USUB16:
        case Aarch32InstructionKind::ARM_INS_USUB8:
        case Aarch32InstructionKind::ARM_INS_UXTAB:
        case Aarch32InstructionKind::ARM_INS_UXTAB16:
        case Aarch32InstructionKind::ARM_INS_UXTAH:
        case Aarch32InstructionKind::ARM_INS_UXTB:
        case Aarch32InstructionKind::ARM_INS_UXTB16:
        case Aarch32InstructionKind::ARM_INS_UXTH:
        case Aarch32InstructionKind::ARM_INS_VLD1:
        case Aarch32InstructionKind::ARM_INS_VLD2:
        case Aarch32InstructionKind::ARM_INS_VLD3:
        case Aarch32InstructionKind::ARM_INS_VLD4:
        case Aarch32InstructionKind::ARM_INS_VLDMDB:
        case Aarch32InstructionKind::ARM_INS_VLDMIA:
        case Aarch32InstructionKind::ARM_INS_VMOV:
        case Aarch32InstructionKind::ARM_INS_VMRS:
        case Aarch32InstructionKind::ARM_INS_VMSR:
        case Aarch32InstructionKind::ARM_INS_VPOP:
        case Aarch32InstructionKind::ARM_INS_VPUSH:
        case Aarch32InstructionKind::ARM_INS_VST1:
        case Aarch32InstructionKind::ARM_INS_VST2:
        case Aarch32InstructionKind::ARM_INS_VST3:
        case Aarch32InstructionKind::ARM_INS_VST4:
        case Aarch32InstructionKind::ARM_INS_VSTMDB:
        case Aarch32InstructionKind::ARM_INS_VSTMIA:
        case Aarch32InstructionKind::ARM_INS_VSTR:
            // These instructions, when writing to the instruction pointer register, have "UNPREDICTABLE" behavior, otherwise
            // they fall through to the next instruction.
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Fall through, or conditionally branch.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_ADD:
        case Aarch32InstructionKind::ARM_INS_ADDW:
        case Aarch32InstructionKind::ARM_INS_ADR:
        case Aarch32InstructionKind::ARM_INS_AND:
        case Aarch32InstructionKind::ARM_INS_ASR:
        case Aarch32InstructionKind::ARM_INS_BIC:
        case Aarch32InstructionKind::ARM_INS_EOR:
        case Aarch32InstructionKind::ARM_INS_LDM:
        case Aarch32InstructionKind::ARM_INS_LDMDA:
        case Aarch32InstructionKind::ARM_INS_LDMDB:
        case Aarch32InstructionKind::ARM_INS_LDMIB:
        case Aarch32InstructionKind::ARM_INS_LDR:
        case Aarch32InstructionKind::ARM_INS_LDRB:                        // FIXME[Robb Matzke 2021-03-01]: recheck up to here, some might be UNPREDICTABLE instead
        case Aarch32InstructionKind::ARM_INS_LSL:
        case Aarch32InstructionKind::ARM_INS_LSR:
        case Aarch32InstructionKind::ARM_INS_MOV:
        case Aarch32InstructionKind::ARM_INS_MOVW:
        case Aarch32InstructionKind::ARM_INS_MRC:
        case Aarch32InstructionKind::ARM_INS_MVN:
        case Aarch32InstructionKind::ARM_INS_ORR:
        case Aarch32InstructionKind::ARM_INS_POP:
        case Aarch32InstructionKind::ARM_INS_ROR:
        case Aarch32InstructionKind::ARM_INS_RRX:
        case Aarch32InstructionKind::ARM_INS_RSB:
        case Aarch32InstructionKind::ARM_INS_RSC:
        case Aarch32InstructionKind::ARM_INS_SBC:
        case Aarch32InstructionKind::ARM_INS_STC:
        case Aarch32InstructionKind::ARM_INS_SUB:
        case Aarch32InstructionKind::ARM_INS_SUBW:
            // These instructions fall through, or branch if they write to the IP.
            //
            // The fall through address is normally the only successor. However, if the instruction writes to the instruction
            // pointer register, then the value written is the basis for the next instruction address. We say "basis" because
            // various additional operations are performed on the address before using it as the next instruciton address
            // (e.g., rounding, changing processor modes, etc).
            //
            // Furthermore, if the instruction writes to the instrunction pointer register and is conditionally executed, then
            // it behaves like a conditional branch.
            //
            // Most of the time, we can't know the resulting value written to the instruction pointer register. The only time
            // we do know is when the constant is available as an immediate value in the instruction encoding, either as an
            // absolute address or a IP-relative address.
            if (insn->get_writesToIp()) {
                if (insn->get_condition() == ARM_CC_AL) {
                    // Acting like an unconditional branch.
                    // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                    return Sawyer::Nothing();
                } else {
                    // Acting like a conditional branch.
                    // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                    return Sawyer::Nothing();
                }
            }
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Explicit branch to possibly known address, or fall through if disabled
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_B:
        case Aarch32InstructionKind::ARM_INS_BL:
        case Aarch32InstructionKind::ARM_INS_BLX:
        case Aarch32InstructionKind::ARM_INS_CBNZ:
        case Aarch32InstructionKind::ARM_INS_CBZ:
            // The branch target is the immediate argument, if present. Some of these instructions can do indirect branching,
            // in which case there won't be an immediate argument.
            for (size_t i = 0; i < insn->nOperands(); ++i) {
                if (isSgAsmIntegerValueExpression(insn->operand(i)))
                    return isSgAsmIntegerValueExpression(insn->operand(i))->get_absoluteValue();
            }
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Explicit branch to impossibly known address, or fall through if disabled
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_BX:
        case Aarch32InstructionKind::ARM_INS_BXJ:
        case Aarch32InstructionKind::ARM_INS_ERET:
        case Aarch32InstructionKind::ARM_INS_RFEDA:
        case Aarch32InstructionKind::ARM_INS_RFEDB:
        case Aarch32InstructionKind::ARM_INS_RFEIA:
        case Aarch32InstructionKind::ARM_INS_RFEIB:
            // Conditional or unconditional branch instructions
            ASSERT_require(insn->get_writesToIp());
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to unknown address
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_BKPT:
        case Aarch32InstructionKind::ARM_INS_HVC:
        case Aarch32InstructionKind::ARM_INS_SMC:
        case Aarch32InstructionKind::ARM_INS_SVC:
        case Aarch32InstructionKind::ARM_INS_TBB:
        case Aarch32InstructionKind::ARM_INS_TBH:
        case Aarch32InstructionKind::ARM_INS_UDF:
        case Aarch32InstructionKind::ARM_INS_WFE:
        case Aarch32InstructionKind::ARM_INS_WFI:
            // We never know where these instructions go next.
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to itself (infininte loop)
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_HLT:
            return insn->get_address();

        //----------------------------------------------------------------------------------------------------
        // Too weird to handle easily.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_IT:
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // These are undocumented (at least in the documentation I have), so assume nothing.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_CDP:
        case Aarch32InstructionKind::ARM_INS_CDP2:
        case Aarch32InstructionKind::ARM_INS_HINT:
        case Aarch32InstructionKind::ARM_INS_LDC2:
        case Aarch32InstructionKind::ARM_INS_LDC2L:
        case Aarch32InstructionKind::ARM_INS_LDCL:
        case Aarch32InstructionKind::ARM_INS_MCR2:
        case Aarch32InstructionKind::ARM_INS_MCRR2:
        case Aarch32InstructionKind::ARM_INS_MRC2:
        case Aarch32InstructionKind::ARM_INS_MRRC2:
        case Aarch32InstructionKind::ARM_INS_STC2:
        case Aarch32InstructionKind::ARM_INS_STC2L:
        case Aarch32InstructionKind::ARM_INS_STCL:
        case Aarch32InstructionKind::ARM_INS_SWP:
        case Aarch32InstructionKind::ARM_INS_SWPB:
        case Aarch32InstructionKind::ARM_INS_TRAP:
            return Sawyer::Nothing();
    }
    ASSERT_not_reachable("insn not handled: " + insn->get_mnemonic());
}

AddressSet
ArmAarch32::getSuccessors(SgAsmInstruction *insn_, bool &complete) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);

    complete = true;                                    // assume true, and prove otherwise
    ASSERT_forbid(insn->get_condition() == ARM_CC_INVALID);
    rose_addr_t fallThroughVa = insn->get_address() + insn->get_size();
    AddressSet retval;
    switch (insn->get_kind()) {
        case Aarch32InstructionKind::ARM_INS_INVALID:
        case Aarch32InstructionKind::ARM_INS_ENDING:
            complete = false;
            break;

        //----------------------------------------------------------------------------------------------------
        // Fall through only.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_AESD:
        case Aarch32InstructionKind::ARM_INS_AESE:
        case Aarch32InstructionKind::ARM_INS_AESIMC:
        case Aarch32InstructionKind::ARM_INS_AESMC:
        case Aarch32InstructionKind::ARM_INS_CLREX:
        case Aarch32InstructionKind::ARM_INS_CMN:
        case Aarch32InstructionKind::ARM_INS_CMP:
        case Aarch32InstructionKind::ARM_INS_CPS:
        case Aarch32InstructionKind::ARM_INS_DBG:
        case Aarch32InstructionKind::ARM_INS_DCPS1:
        case Aarch32InstructionKind::ARM_INS_DCPS2:
        case Aarch32InstructionKind::ARM_INS_DCPS3:
        case Aarch32InstructionKind::ARM_INS_DMB:
        case Aarch32InstructionKind::ARM_INS_DSB:
        case Aarch32InstructionKind::ARM_INS_ISB:
        case Aarch32InstructionKind::ARM_INS_LDC:
        case Aarch32InstructionKind::ARM_INS_MCR:
        case Aarch32InstructionKind::ARM_INS_MCRR:
        case Aarch32InstructionKind::ARM_INS_MSR:
        case Aarch32InstructionKind::ARM_INS_NOP:
        case Aarch32InstructionKind::ARM_INS_PLD:
        case Aarch32InstructionKind::ARM_INS_PLDW:
        case Aarch32InstructionKind::ARM_INS_PLI:
        case Aarch32InstructionKind::ARM_INS_PUSH:
        case Aarch32InstructionKind::ARM_INS_SETEND:
        case Aarch32InstructionKind::ARM_INS_SEV:
        case Aarch32InstructionKind::ARM_INS_SEVL:
        case Aarch32InstructionKind::ARM_INS_SRSDA:
        case Aarch32InstructionKind::ARM_INS_SRSDB:
        case Aarch32InstructionKind::ARM_INS_SRSIA:
        case Aarch32InstructionKind::ARM_INS_SRSIB:
        case Aarch32InstructionKind::ARM_INS_TEQ:
        case Aarch32InstructionKind::ARM_INS_TST:
        case Aarch32InstructionKind::ARM_INS_YIELD:
        case Aarch32InstructionKind::ARM_INS_VABA:
        case Aarch32InstructionKind::ARM_INS_VABAL:
        case Aarch32InstructionKind::ARM_INS_VABD:
        case Aarch32InstructionKind::ARM_INS_VABDL:
        case Aarch32InstructionKind::ARM_INS_VABS:
        case Aarch32InstructionKind::ARM_INS_VACGE:
        case Aarch32InstructionKind::ARM_INS_VACGT:
        case Aarch32InstructionKind::ARM_INS_VADD:
        case Aarch32InstructionKind::ARM_INS_VADDHN:
        case Aarch32InstructionKind::ARM_INS_VADDL:
        case Aarch32InstructionKind::ARM_INS_VADDW:
        case Aarch32InstructionKind::ARM_INS_VAND:
        case Aarch32InstructionKind::ARM_INS_VBIC:
        case Aarch32InstructionKind::ARM_INS_VBIF:
        case Aarch32InstructionKind::ARM_INS_VBIT:
        case Aarch32InstructionKind::ARM_INS_VBSL:
        case Aarch32InstructionKind::ARM_INS_VCEQ:
        case Aarch32InstructionKind::ARM_INS_VCGE:
        case Aarch32InstructionKind::ARM_INS_VCGT:
        case Aarch32InstructionKind::ARM_INS_VCLE:
        case Aarch32InstructionKind::ARM_INS_VCLS:
        case Aarch32InstructionKind::ARM_INS_VCLT:
        case Aarch32InstructionKind::ARM_INS_VCLZ:
        case Aarch32InstructionKind::ARM_INS_VCMP:
        case Aarch32InstructionKind::ARM_INS_VCMPE:
        case Aarch32InstructionKind::ARM_INS_VCNT:
        case Aarch32InstructionKind::ARM_INS_VCVT:
        case Aarch32InstructionKind::ARM_INS_VCVTA:
        case Aarch32InstructionKind::ARM_INS_VCVTB:
        case Aarch32InstructionKind::ARM_INS_VCVTM:
        case Aarch32InstructionKind::ARM_INS_VCVTN:
        case Aarch32InstructionKind::ARM_INS_VCVTP:
        case Aarch32InstructionKind::ARM_INS_VCVTR:
        case Aarch32InstructionKind::ARM_INS_VCVTT:
        case Aarch32InstructionKind::ARM_INS_VDIV:
        case Aarch32InstructionKind::ARM_INS_VDUP:
        case Aarch32InstructionKind::ARM_INS_VEOR:
        case Aarch32InstructionKind::ARM_INS_VEXT:
        case Aarch32InstructionKind::ARM_INS_VFMA:
        case Aarch32InstructionKind::ARM_INS_VFMS:
        case Aarch32InstructionKind::ARM_INS_VFNMA:
        case Aarch32InstructionKind::ARM_INS_VFNMS:
        case Aarch32InstructionKind::ARM_INS_VHADD:
        case Aarch32InstructionKind::ARM_INS_VHSUB:
        case Aarch32InstructionKind::ARM_INS_VLDR:
        case Aarch32InstructionKind::ARM_INS_VMAX:
        case Aarch32InstructionKind::ARM_INS_VMAXNM:
        case Aarch32InstructionKind::ARM_INS_VMIN:
        case Aarch32InstructionKind::ARM_INS_VMINNM:
        case Aarch32InstructionKind::ARM_INS_VMLA:
        case Aarch32InstructionKind::ARM_INS_VMLAL:
        case Aarch32InstructionKind::ARM_INS_VMLS:
        case Aarch32InstructionKind::ARM_INS_VMLSL:
        case Aarch32InstructionKind::ARM_INS_VMOVL:
        case Aarch32InstructionKind::ARM_INS_VMOVN:
        case Aarch32InstructionKind::ARM_INS_VMUL:
        case Aarch32InstructionKind::ARM_INS_VMULL:
        case Aarch32InstructionKind::ARM_INS_VMVN:
        case Aarch32InstructionKind::ARM_INS_VNEG:
        case Aarch32InstructionKind::ARM_INS_VNMLA:
        case Aarch32InstructionKind::ARM_INS_VNMLS:
        case Aarch32InstructionKind::ARM_INS_VNMUL:
        case Aarch32InstructionKind::ARM_INS_VORN:
        case Aarch32InstructionKind::ARM_INS_VORR:
        case Aarch32InstructionKind::ARM_INS_VPADAL:
        case Aarch32InstructionKind::ARM_INS_VPADD:
        case Aarch32InstructionKind::ARM_INS_VPADDL:
        case Aarch32InstructionKind::ARM_INS_VPMAX:
        case Aarch32InstructionKind::ARM_INS_VPMIN:
        case Aarch32InstructionKind::ARM_INS_VQABS:
        case Aarch32InstructionKind::ARM_INS_VQADD:
        case Aarch32InstructionKind::ARM_INS_VQDMLAL:
        case Aarch32InstructionKind::ARM_INS_VQDMLSL:
        case Aarch32InstructionKind::ARM_INS_VQDMULH:
        case Aarch32InstructionKind::ARM_INS_VQDMULL:
        case Aarch32InstructionKind::ARM_INS_VQMOVN:
        case Aarch32InstructionKind::ARM_INS_VQMOVUN:
        case Aarch32InstructionKind::ARM_INS_VQNEG:
        case Aarch32InstructionKind::ARM_INS_VQRDMULH:
        case Aarch32InstructionKind::ARM_INS_VQRSHL:
        case Aarch32InstructionKind::ARM_INS_VQRSHRN:
        case Aarch32InstructionKind::ARM_INS_VQRSHRUN:
        case Aarch32InstructionKind::ARM_INS_VQSHL:
        case Aarch32InstructionKind::ARM_INS_VQSHLU:
        case Aarch32InstructionKind::ARM_INS_VQSHRN:
        case Aarch32InstructionKind::ARM_INS_VQSHRUN:
        case Aarch32InstructionKind::ARM_INS_VQSUB:
        case Aarch32InstructionKind::ARM_INS_VRADDHN:
        case Aarch32InstructionKind::ARM_INS_VRECPE:
        case Aarch32InstructionKind::ARM_INS_VRECPS:
        case Aarch32InstructionKind::ARM_INS_VREV16:
        case Aarch32InstructionKind::ARM_INS_VREV32:
        case Aarch32InstructionKind::ARM_INS_VREV64:
        case Aarch32InstructionKind::ARM_INS_VRHADD:
        case Aarch32InstructionKind::ARM_INS_VRINTA:
        case Aarch32InstructionKind::ARM_INS_VRINTM:
        case Aarch32InstructionKind::ARM_INS_VRINTN:
        case Aarch32InstructionKind::ARM_INS_VRINTP:
        case Aarch32InstructionKind::ARM_INS_VRINTR:
        case Aarch32InstructionKind::ARM_INS_VRINTX:
        case Aarch32InstructionKind::ARM_INS_VRINTZ:
        case Aarch32InstructionKind::ARM_INS_VRSHL:
        case Aarch32InstructionKind::ARM_INS_VRSHR:
        case Aarch32InstructionKind::ARM_INS_VRSHRN:
        case Aarch32InstructionKind::ARM_INS_VRSQRTE:
        case Aarch32InstructionKind::ARM_INS_VRSQRTS:
        case Aarch32InstructionKind::ARM_INS_VRSRA:
        case Aarch32InstructionKind::ARM_INS_VRSUBHN:
        case Aarch32InstructionKind::ARM_INS_VSELEQ:
        case Aarch32InstructionKind::ARM_INS_VSELGE:
        case Aarch32InstructionKind::ARM_INS_VSELGT:
        case Aarch32InstructionKind::ARM_INS_VSELVS:
        case Aarch32InstructionKind::ARM_INS_VSHL:
        case Aarch32InstructionKind::ARM_INS_VSHLL:
        case Aarch32InstructionKind::ARM_INS_VSHR:
        case Aarch32InstructionKind::ARM_INS_VSHRN:
        case Aarch32InstructionKind::ARM_INS_VSLI:
        case Aarch32InstructionKind::ARM_INS_VSQRT:
        case Aarch32InstructionKind::ARM_INS_VSRA:
        case Aarch32InstructionKind::ARM_INS_VSRI:
        case Aarch32InstructionKind::ARM_INS_VSUB:
        case Aarch32InstructionKind::ARM_INS_VSUBHN:
        case Aarch32InstructionKind::ARM_INS_VSUBL:
        case Aarch32InstructionKind::ARM_INS_VSUBW:
        case Aarch32InstructionKind::ARM_INS_VSWP:
        case Aarch32InstructionKind::ARM_INS_VTBL:
        case Aarch32InstructionKind::ARM_INS_VTBX:
        case Aarch32InstructionKind::ARM_INS_VTRN:
        case Aarch32InstructionKind::ARM_INS_VTST:
        case Aarch32InstructionKind::ARM_INS_VUZP:
        case Aarch32InstructionKind::ARM_INS_VZIP:
            // These instructions always fall through to the next instruction, and never anything else. They cannot write to
            // the IP.
            ASSERT_forbid(insn->get_writesToIp());
            retval.insert(fallThroughVa);
            break;

        //----------------------------------------------------------------------------------------------------
        // Fall through, or UNPREDICTABLE behavior.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_ADC:
        case Aarch32InstructionKind::ARM_INS_BFC:
        case Aarch32InstructionKind::ARM_INS_BFI:
        case Aarch32InstructionKind::ARM_INS_CLZ:
        case Aarch32InstructionKind::ARM_INS_CRC32B:
        case Aarch32InstructionKind::ARM_INS_CRC32H:
        case Aarch32InstructionKind::ARM_INS_CRC32W:
        case Aarch32InstructionKind::ARM_INS_CRC32CB:
        case Aarch32InstructionKind::ARM_INS_CRC32CH:
        case Aarch32InstructionKind::ARM_INS_CRC32CW:
        case Aarch32InstructionKind::ARM_INS_FLDMDBX:
        case Aarch32InstructionKind::ARM_INS_FLDMIAX:
        case Aarch32InstructionKind::ARM_INS_FSTMDBX:
        case Aarch32InstructionKind::ARM_INS_FSTMIAX:
        case Aarch32InstructionKind::ARM_INS_LDA:
        case Aarch32InstructionKind::ARM_INS_LDAB:
        case Aarch32InstructionKind::ARM_INS_LDAEX:
        case Aarch32InstructionKind::ARM_INS_LDAEXB:
        case Aarch32InstructionKind::ARM_INS_LDAEXD:
        case Aarch32InstructionKind::ARM_INS_LDAEXH:
        case Aarch32InstructionKind::ARM_INS_LDAH:
        case Aarch32InstructionKind::ARM_INS_LDRBT:
        case Aarch32InstructionKind::ARM_INS_LDRD:
        case Aarch32InstructionKind::ARM_INS_LDREX:
        case Aarch32InstructionKind::ARM_INS_LDREXB:
        case Aarch32InstructionKind::ARM_INS_LDREXD:
        case Aarch32InstructionKind::ARM_INS_LDREXH:
        case Aarch32InstructionKind::ARM_INS_LDRH:
        case Aarch32InstructionKind::ARM_INS_LDRHT:
        case Aarch32InstructionKind::ARM_INS_LDRSB:
        case Aarch32InstructionKind::ARM_INS_LDRSBT:
        case Aarch32InstructionKind::ARM_INS_LDRSH:
        case Aarch32InstructionKind::ARM_INS_LDRSHT:
        case Aarch32InstructionKind::ARM_INS_LDRT:
        case Aarch32InstructionKind::ARM_INS_MLA:
        case Aarch32InstructionKind::ARM_INS_MLS:
        case Aarch32InstructionKind::ARM_INS_MOVT:
        case Aarch32InstructionKind::ARM_INS_MRRC:
        case Aarch32InstructionKind::ARM_INS_MRS:
        case Aarch32InstructionKind::ARM_INS_MUL:
        case Aarch32InstructionKind::ARM_INS_ORN:
        case Aarch32InstructionKind::ARM_INS_PKHBT:
        case Aarch32InstructionKind::ARM_INS_PKHTB:
        case Aarch32InstructionKind::ARM_INS_QADD:
        case Aarch32InstructionKind::ARM_INS_QADD16:
        case Aarch32InstructionKind::ARM_INS_QADD8:
        case Aarch32InstructionKind::ARM_INS_QASX:
        case Aarch32InstructionKind::ARM_INS_QDADD:
        case Aarch32InstructionKind::ARM_INS_QDSUB:
        case Aarch32InstructionKind::ARM_INS_QSAX:
        case Aarch32InstructionKind::ARM_INS_QSUB:
        case Aarch32InstructionKind::ARM_INS_QSUB16:
        case Aarch32InstructionKind::ARM_INS_QSUB8:
        case Aarch32InstructionKind::ARM_INS_RBIT:
        case Aarch32InstructionKind::ARM_INS_REV:
        case Aarch32InstructionKind::ARM_INS_REV16:
        case Aarch32InstructionKind::ARM_INS_REVSH:
        case Aarch32InstructionKind::ARM_INS_SADD16:
        case Aarch32InstructionKind::ARM_INS_SADD8:
        case Aarch32InstructionKind::ARM_INS_SASX:
        case Aarch32InstructionKind::ARM_INS_SBFX:
        case Aarch32InstructionKind::ARM_INS_SDIV:
        case Aarch32InstructionKind::ARM_INS_SEL:
        case Aarch32InstructionKind::ARM_INS_SHA1C:
        case Aarch32InstructionKind::ARM_INS_SHA1H:
        case Aarch32InstructionKind::ARM_INS_SHA1M:
        case Aarch32InstructionKind::ARM_INS_SHA1P:
        case Aarch32InstructionKind::ARM_INS_SHA1SU0:
        case Aarch32InstructionKind::ARM_INS_SHA1SU1:
        case Aarch32InstructionKind::ARM_INS_SHA256H:
        case Aarch32InstructionKind::ARM_INS_SHA256H2:
        case Aarch32InstructionKind::ARM_INS_SHA256SU0:
        case Aarch32InstructionKind::ARM_INS_SHA256SU1:
        case Aarch32InstructionKind::ARM_INS_SHADD16:
        case Aarch32InstructionKind::ARM_INS_SHADD8:
        case Aarch32InstructionKind::ARM_INS_SHASX:
        case Aarch32InstructionKind::ARM_INS_SHSAX:
        case Aarch32InstructionKind::ARM_INS_SHSUB16:
        case Aarch32InstructionKind::ARM_INS_SHSUB8:
        case Aarch32InstructionKind::ARM_INS_SMLABB:
        case Aarch32InstructionKind::ARM_INS_SMLABT:
        case Aarch32InstructionKind::ARM_INS_SMLATB:
        case Aarch32InstructionKind::ARM_INS_SMLATT:
        case Aarch32InstructionKind::ARM_INS_SMLAD:
        case Aarch32InstructionKind::ARM_INS_SMLADX:
        case Aarch32InstructionKind::ARM_INS_SMLAL:
        case Aarch32InstructionKind::ARM_INS_SMLALBB:
        case Aarch32InstructionKind::ARM_INS_SMLALBT:
        case Aarch32InstructionKind::ARM_INS_SMLALTB:
        case Aarch32InstructionKind::ARM_INS_SMLALTT:
        case Aarch32InstructionKind::ARM_INS_SMLALD:
        case Aarch32InstructionKind::ARM_INS_SMLALDX:
        case Aarch32InstructionKind::ARM_INS_SMLAWB:
        case Aarch32InstructionKind::ARM_INS_SMLAWT:
        case Aarch32InstructionKind::ARM_INS_SMLSD:
        case Aarch32InstructionKind::ARM_INS_SMLSDX:
        case Aarch32InstructionKind::ARM_INS_SMLSLD:
        case Aarch32InstructionKind::ARM_INS_SMLSLDX:
        case Aarch32InstructionKind::ARM_INS_SMMLA:
        case Aarch32InstructionKind::ARM_INS_SMMLAR:
        case Aarch32InstructionKind::ARM_INS_SMMLS:
        case Aarch32InstructionKind::ARM_INS_SMMLSR:
        case Aarch32InstructionKind::ARM_INS_SMMUL:
        case Aarch32InstructionKind::ARM_INS_SMMULR:
        case Aarch32InstructionKind::ARM_INS_SMUAD:
        case Aarch32InstructionKind::ARM_INS_SMUADX:
        case Aarch32InstructionKind::ARM_INS_SMULBB:
        case Aarch32InstructionKind::ARM_INS_SMULBT:
        case Aarch32InstructionKind::ARM_INS_SMULTB:
        case Aarch32InstructionKind::ARM_INS_SMULTT:
        case Aarch32InstructionKind::ARM_INS_SMULL:
        case Aarch32InstructionKind::ARM_INS_SMULWB:
        case Aarch32InstructionKind::ARM_INS_SMULWT:
        case Aarch32InstructionKind::ARM_INS_SMUSD:
        case Aarch32InstructionKind::ARM_INS_SMUSDX:
        case Aarch32InstructionKind::ARM_INS_SSAT:
        case Aarch32InstructionKind::ARM_INS_SSAT16:
        case Aarch32InstructionKind::ARM_INS_SSAX:
        case Aarch32InstructionKind::ARM_INS_SSUB16:
        case Aarch32InstructionKind::ARM_INS_SSUB8:
        case Aarch32InstructionKind::ARM_INS_STL:
        case Aarch32InstructionKind::ARM_INS_STLB:
        case Aarch32InstructionKind::ARM_INS_STLEX:
        case Aarch32InstructionKind::ARM_INS_STLEXB:
        case Aarch32InstructionKind::ARM_INS_STLEXD:
        case Aarch32InstructionKind::ARM_INS_STLEXH:
        case Aarch32InstructionKind::ARM_INS_STLH:
        case Aarch32InstructionKind::ARM_INS_STM:
        case Aarch32InstructionKind::ARM_INS_STMDA:
        case Aarch32InstructionKind::ARM_INS_STMDB:
        case Aarch32InstructionKind::ARM_INS_STMIB:
        case Aarch32InstructionKind::ARM_INS_STR:
        case Aarch32InstructionKind::ARM_INS_STRB:
        case Aarch32InstructionKind::ARM_INS_STRBT:
        case Aarch32InstructionKind::ARM_INS_STRD:
        case Aarch32InstructionKind::ARM_INS_STREX:
        case Aarch32InstructionKind::ARM_INS_STREXB:
        case Aarch32InstructionKind::ARM_INS_STREXD:
        case Aarch32InstructionKind::ARM_INS_STREXH:
        case Aarch32InstructionKind::ARM_INS_STRH:
        case Aarch32InstructionKind::ARM_INS_STRHT:
        case Aarch32InstructionKind::ARM_INS_STRT:
        case Aarch32InstructionKind::ARM_INS_SXTAB:
        case Aarch32InstructionKind::ARM_INS_SXTAB16:
        case Aarch32InstructionKind::ARM_INS_SXTAH:
        case Aarch32InstructionKind::ARM_INS_SXTB:
        case Aarch32InstructionKind::ARM_INS_SXTB16:
        case Aarch32InstructionKind::ARM_INS_SXTH:
        case Aarch32InstructionKind::ARM_INS_UADD16:
        case Aarch32InstructionKind::ARM_INS_UADD8:
        case Aarch32InstructionKind::ARM_INS_UASX:
        case Aarch32InstructionKind::ARM_INS_UBFX:
        case Aarch32InstructionKind::ARM_INS_UDIV:
        case Aarch32InstructionKind::ARM_INS_UHADD16:
        case Aarch32InstructionKind::ARM_INS_UHADD8:
        case Aarch32InstructionKind::ARM_INS_UHASX:
        case Aarch32InstructionKind::ARM_INS_UHSAX:
        case Aarch32InstructionKind::ARM_INS_UHSUB16:
        case Aarch32InstructionKind::ARM_INS_UHSUB8:
        case Aarch32InstructionKind::ARM_INS_UMAAL:
        case Aarch32InstructionKind::ARM_INS_UMLAL:
        case Aarch32InstructionKind::ARM_INS_UMULL:
        case Aarch32InstructionKind::ARM_INS_UQADD16:
        case Aarch32InstructionKind::ARM_INS_UQADD8:
        case Aarch32InstructionKind::ARM_INS_UQASX:
        case Aarch32InstructionKind::ARM_INS_UQSAX:
        case Aarch32InstructionKind::ARM_INS_UQSUB16:
        case Aarch32InstructionKind::ARM_INS_UQSUB8:
        case Aarch32InstructionKind::ARM_INS_USAD8:
        case Aarch32InstructionKind::ARM_INS_USADA8:
        case Aarch32InstructionKind::ARM_INS_USAT:
        case Aarch32InstructionKind::ARM_INS_USAT16:
        case Aarch32InstructionKind::ARM_INS_USAX:
        case Aarch32InstructionKind::ARM_INS_USUB16:
        case Aarch32InstructionKind::ARM_INS_USUB8:
        case Aarch32InstructionKind::ARM_INS_UXTAB:
        case Aarch32InstructionKind::ARM_INS_UXTAB16:
        case Aarch32InstructionKind::ARM_INS_UXTAH:
        case Aarch32InstructionKind::ARM_INS_UXTB:
        case Aarch32InstructionKind::ARM_INS_UXTB16:
        case Aarch32InstructionKind::ARM_INS_UXTH:
        case Aarch32InstructionKind::ARM_INS_VLD1:
        case Aarch32InstructionKind::ARM_INS_VLD2:
        case Aarch32InstructionKind::ARM_INS_VLD3:
        case Aarch32InstructionKind::ARM_INS_VLD4:
        case Aarch32InstructionKind::ARM_INS_VLDMDB:
        case Aarch32InstructionKind::ARM_INS_VLDMIA:
        case Aarch32InstructionKind::ARM_INS_VMOV:
        case Aarch32InstructionKind::ARM_INS_VMRS:
        case Aarch32InstructionKind::ARM_INS_VMSR:
        case Aarch32InstructionKind::ARM_INS_VPOP:
        case Aarch32InstructionKind::ARM_INS_VPUSH:
        case Aarch32InstructionKind::ARM_INS_VST1:
        case Aarch32InstructionKind::ARM_INS_VST2:
        case Aarch32InstructionKind::ARM_INS_VST3:
        case Aarch32InstructionKind::ARM_INS_VST4:
        case Aarch32InstructionKind::ARM_INS_VSTMDB:
        case Aarch32InstructionKind::ARM_INS_VSTMIA:
        case Aarch32InstructionKind::ARM_INS_VSTR:
            // These instructions, when writing to the instruction pointer register, have "UNPREDICTABLE" behavior, otherwise
            // they fall through to the next instruction.
            if (insn->get_writesToIp()) {
                complete = false;
            } else {
                retval.insert(fallThroughVa);
            }
            break;

        //----------------------------------------------------------------------------------------------------
        // Fall through, or conditionally branch.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_ADD:
        case Aarch32InstructionKind::ARM_INS_ADDW:
        case Aarch32InstructionKind::ARM_INS_ADR:
        case Aarch32InstructionKind::ARM_INS_AND:
        case Aarch32InstructionKind::ARM_INS_ASR:
        case Aarch32InstructionKind::ARM_INS_BIC:
        case Aarch32InstructionKind::ARM_INS_EOR:
        case Aarch32InstructionKind::ARM_INS_LDM:
        case Aarch32InstructionKind::ARM_INS_LDMDA:
        case Aarch32InstructionKind::ARM_INS_LDMDB:
        case Aarch32InstructionKind::ARM_INS_LDMIB:
        case Aarch32InstructionKind::ARM_INS_LDR:
        case Aarch32InstructionKind::ARM_INS_LDRB:                        // FIXME[Robb Matzke 2021-03-01]: recheck up to here, some might be UNPREDICTABLE instead
        case Aarch32InstructionKind::ARM_INS_LSL:
        case Aarch32InstructionKind::ARM_INS_LSR:
        case Aarch32InstructionKind::ARM_INS_MOV:
        case Aarch32InstructionKind::ARM_INS_MOVW:
        case Aarch32InstructionKind::ARM_INS_MRC:
        case Aarch32InstructionKind::ARM_INS_MVN:
        case Aarch32InstructionKind::ARM_INS_ORR:
        case Aarch32InstructionKind::ARM_INS_POP:
        case Aarch32InstructionKind::ARM_INS_ROR:
        case Aarch32InstructionKind::ARM_INS_RRX:
        case Aarch32InstructionKind::ARM_INS_RSB:
        case Aarch32InstructionKind::ARM_INS_RSC:
        case Aarch32InstructionKind::ARM_INS_SBC:
        case Aarch32InstructionKind::ARM_INS_STC:
        case Aarch32InstructionKind::ARM_INS_SUB:
        case Aarch32InstructionKind::ARM_INS_SUBW:
            // These instructions fall through, or branch if they write to the IP.
            //
            // The fall through address is normally the only successor. However, if the instruction writes to the instruction
            // pointer register, then the value written is the basis for the next instruction address. We say "basis" because
            // various additional operations are performed on the address before using it as the next instruciton address
            // (e.g., rounding, changing processor modes, etc).
            //
            // Furthermore, if the instruction writes to the instrunction pointer register and is conditionally executed, then
            // it behaves like a conditional branch.
            //
            // Most of the time, we can't know the resulting value written to the instruction pointer register. The only time
            // we do know is when the constant is available as an immediate value in the instruction encoding, either as an
            // absolute address or a IP-relative address.
            if (insn->get_writesToIp()) {
                if (insn->get_condition() == ARM_CC_AL) {
                    // Acting like an unconditional branch.
                    complete = false;                   // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                } else {
                    // Acting like a conditional branch.
                    complete = false;                   // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                    retval.insert(fallThroughVa);
                }
            } else {
                retval.insert(fallThroughVa);
            }
            break;

        //----------------------------------------------------------------------------------------------------
        // Explicit branch (known or unknown target), or fall through if disabled
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_B:
        case Aarch32InstructionKind::ARM_INS_BL:
        case Aarch32InstructionKind::ARM_INS_BLX:
        case Aarch32InstructionKind::ARM_INS_BX:
        case Aarch32InstructionKind::ARM_INS_BXJ:
        case Aarch32InstructionKind::ARM_INS_CBNZ:
        case Aarch32InstructionKind::ARM_INS_CBZ:
        case Aarch32InstructionKind::ARM_INS_ERET:
        case Aarch32InstructionKind::ARM_INS_RFEDA:
        case Aarch32InstructionKind::ARM_INS_RFEDB:
        case Aarch32InstructionKind::ARM_INS_RFEIA:
        case Aarch32InstructionKind::ARM_INS_RFEIB:
            // Conditional or unconditional branch instructions
            ASSERT_require(insn->get_writesToIp());
            if (auto target = branchTarget(insn)) {
                retval.insert(*target);
            } else {
                complete = false;
            }
            if (insn->get_condition() != ARM_CC_AL)
                retval.insert(fallThroughVa);
            break;

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to unknown address
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_BKPT:
        case Aarch32InstructionKind::ARM_INS_HVC:
        case Aarch32InstructionKind::ARM_INS_SMC:
        case Aarch32InstructionKind::ARM_INS_SVC:
        case Aarch32InstructionKind::ARM_INS_TBB:
        case Aarch32InstructionKind::ARM_INS_TBH:
        case Aarch32InstructionKind::ARM_INS_UDF:
        case Aarch32InstructionKind::ARM_INS_WFE:
        case Aarch32InstructionKind::ARM_INS_WFI:
            // We never know where these instructions go next.
            complete = false;
            break;

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to itself (infininte loop)
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_HLT:
            retval.insert(insn->get_address());
            break;

        //----------------------------------------------------------------------------------------------------
        // Too weird to handle easily.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_IT:
            complete = false;
            break;

        //----------------------------------------------------------------------------------------------------
        // These are undocumented (at least in the documentation I have), so assume nothing.
        //----------------------------------------------------------------------------------------------------
        case Aarch32InstructionKind::ARM_INS_CDP:
        case Aarch32InstructionKind::ARM_INS_CDP2:
        case Aarch32InstructionKind::ARM_INS_HINT:
        case Aarch32InstructionKind::ARM_INS_LDC2:
        case Aarch32InstructionKind::ARM_INS_LDC2L:
        case Aarch32InstructionKind::ARM_INS_LDCL:
        case Aarch32InstructionKind::ARM_INS_MCR2:
        case Aarch32InstructionKind::ARM_INS_MCRR2:
        case Aarch32InstructionKind::ARM_INS_MRC2:
        case Aarch32InstructionKind::ARM_INS_MRRC2:
        case Aarch32InstructionKind::ARM_INS_STC2:
        case Aarch32InstructionKind::ARM_INS_STC2L:
        case Aarch32InstructionKind::ARM_INS_STCL:
        case Aarch32InstructionKind::ARM_INS_SWP:
        case Aarch32InstructionKind::ARM_INS_SWPB:
        case Aarch32InstructionKind::ARM_INS_TRAP:
            complete = false;
            break;
    }
    return retval;
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
