#ifndef ROSE_BinaryAnalysis_InstructionEnumsAarch32_H
#define ROSE_BinaryAnalysis_InstructionEnumsAarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <capstone/arm.h>

namespace Rose {
namespace BinaryAnalysis {

using ::arm_insn;                                       // from capstone
using Aarch32InstructionKind = ::arm_insn;              /**< ARM A32 and T32 instruction types. */

using ::arm_cc;
using Aarch32InstructionCondition = ::arm_cc;           /**< Aarch32 condition codes. */

// Exception major numbers.
enum Aarch32Exception {
    aarch32_exception_svc,                              // Exception for the SVC instruction.
    aarch32_exception_udf,                              // Exception for the UDF instruction.
    aarch32_exception_hvc                               // Exception for hypervisor calls.
};

// Major register numbers for AArch32.
enum Aarch32RegisterClass {
    aarch32_regclass_gpr,                               // General purpose registers.
    aarch32_regclass_sys,                               // Special purpose, non-user registers.
    aarch32_regclass_ext,                               // NEON and VFP extension register bank.
    aarch32_regclass_coproc,                            // Coprocessor.
    aarch32_regclass_debug                              // Debug registers.
};

// General purpose registers for AArch32.
enum Aarch32GeneralPurposeRegister {
    aarch32_gpr_sb = 9,
    aarch32_gpr_sl = 10,
    aarch32_gpr_fp = 11,
    aarch32_gpr_ip = 12,                                // not the instruction pointer
    aarch32_gpr_sp = 13,                                // User stack pointer. Not the priviledged stack pointers.
    aarch32_gpr_lr = 14,                                // User link register. Not the privileged link registers.
    aarch32_gpr_pc = 15
};

// Debug registers for AArch32.
enum Aarch32DebugRegister {
    // These numbers are specified in the ARM documentation
    aarch32_debug_didr = 0,                             // Debug ID register.
    // 1 - 5 is reserved
    aarch32_debug_wfar = 6,                             // Watchpoint fault address register.
    aarch32_debug_vcr = 7,                              // Vector catch register.
    // 8 reserved
    aarch32_debug_ecr = 9,                              // Event catch register.
    aarch32_debug_dsccr = 10,                           // Debug state catch control register.
    aarch32_debug_dsmcr = 11,                           // Debug state MMU control register.
    // 12 - 31 are reserved
    aarch32_debug_dtrrx = 32,                           // DTRRX external view.
    aarch32_debug_itr = 33,                             // Instruction transfer register.
    aarch32_debug_dscr = 34,                            // DSCR external view.
    aarch32_debug_dtrtx = 35,                           // DTRTX external view.
    aarch32_debug_drcr = 36,                            // Debug run control register.
    // 37 - 63 are reserved
    aarch32_debug_bvr0 = 64,                            // First breakpoint value register. There are 16 of these.
    // 65 - 79 are the other 15 BVR registers
    aarch32_debug_bcr0 = 80,                            // First breakpiont control register. There are 16 of these.
    // 81 - 95 are the other 15 BCR registers
    aarch32_debug_wvr0 = 96,                            // First watchpoint value register. There are 16 of these.
    // 97 - 111 are the other 15 WVR registers
    aarch32_debug_wcr0 = 112,                           // First watchpoint control register. There are 16 of these.
    // 128 - 191 are reserved
    aarch32_debug_oslar = 192,                          // OS lock access register.
    aarch32_debug_oslsr = 193,                          // OS lock status register.
    aarch32_debug_ossrr = 194,                          // OS save and restore register.
    // 195 reserved
    aarch32_debug_prcr = 196,                           // Device power-down and reset control register.
    aarch32_debug_prsr = 197,                           // Device power-down and reset status register.
    // 198 - 511 are reserved
    // 512 - 575 are implementation defined
    // 576 - 831 are reserved
    // 832 - 895 are processor identification registers (no names given in documentation)
    // 896 - 927 are reserved
    // 928 - 959 are implementation defined
    aarch32_debug_itctrl = 960,                         // Integration mode control register.
    // 961 -999 are reserved for management register expansion
    aarch32_debug_claimset = 1000,                      // Claim tag set register.
    aarch32_debug_claimclr = 1001,                      // Claim tag clear register.
    // 1002 - 1003 are reserved
    aarch32_debug_lar = 1004,                           // Lock access register.
    aarch32_debug_lsr = 1005,                           // Lock status register.
    aarch32_debug_authstatus = 1006,                    // Authentication status register.
    // 1007 - 1009 are reserved
    aarch32_debug_devid = 1010,                         // Device identifier.
    aarch32_debug_devtype = 1011,                       // Device type register.
    aarch32_debug_peripheralid0 = 1012,                 // Peripheral identification registers. There are 8 of these.
    aarch32_debug_componentid0 = 1020,                  // Component identification registers. There are 4 of these.

    // These registers have their register number specified as "N/A" in the documentation: "These registers are only
    // implemented through the Baseline CP14 Interface and do not have register numbers or offsets."
    aarch32_debug_drar = 2000,
    aarch32_debug_dsar
};

// System (non-user) registers for AArch32.
enum Aarch32SystemRegister {
    aarch32_sys_r8_usr,
    aarch32_sys_r8_fiq,
    aarch32_sys_r9_usr,
    aarch32_sys_r9_fiq,
    aarch32_sys_r10_usr,
    aarch32_sys_r10_fiq,
    aarch32_sys_r11_usr,
    aarch32_sys_r11_fiq,
    aarch32_sys_r12_usr,
    aarch32_sys_r12_fiq,
    aarch32_sys_sp_hyp,
    aarch32_sys_sp_svc,
    aarch32_sys_sp_abt,
    aarch32_sys_sp_und,
    aarch32_sys_sp_mon,
    aarch32_sys_sp_irq,
    aarch32_sys_sp_fiq,
    aarch32_sys_sp_usr,
    aarch32_sys_lr_svc,
    aarch32_sys_lr_abt,
    aarch32_sys_lr_und,
    aarch32_sys_lr_mon,
    aarch32_sys_lr_irq,
    aarch32_sys_lr_fiq,
    aarch32_sys_lr_usr,
    aarch32_sys_cpsr,
    aarch32_sys_apsr,
    aarch32_sys_spsr,                                   // not really a hardware register: stand in for aarch32_sys_spsr_*
    aarch32_sys_spsr_hyp,
    aarch32_sys_spsr_svc,
    aarch32_sys_spsr_abt,
    aarch32_sys_spsr_und,
    aarch32_sys_spsr_mon,
    aarch32_sys_spsr_irq,
    aarch32_sys_spsr_fiq,
    aarch32_sys_ipsr,
    aarch32_sys_iepsr,
    aarch32_sys_iapsr,
    aarch32_sys_eapsr,
    aarch32_sys_psr,
    aarch32_sys_msp,
    aarch32_sys_psp,
    aarch32_sys_primask,
    aarch32_sys_control,
    aarch32_sys_fpsid,
    aarch32_sys_fpscr,
    aarch32_sys_fpexc,
    aarch32_sys_fpinst,
    aarch32_sys_fpinst2,
    aarch32_sys_mvfr0,
    aarch32_sys_mvfr1,
    aarch32_sys_mvfr2,
    aarch32_sys_itstate,
    aarch32_sys_unknown                                 // not really a hardware register; used internally by ROSE
};

} // namespace
} // namespace

#endif
#endif
