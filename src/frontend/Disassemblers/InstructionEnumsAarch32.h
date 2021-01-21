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

/** Major register numbers for AArch32. */
enum Aarch32RegisterClass {
    aarch32_regclass_gpr,                               /**< General purpose registers. */
    aarch32_regclass_sys,                               /**< Special purpose, non-user registers. */
    aarch32_regclass_ext,                               /**< NEON and VFP extension register bank. */
    aarch32_regclass_coproc                             /**< Coprocessor. */
};

/** General purpose registers for AArch32. */
enum Aarch32GeneralPurposeRegister {
    aarch32_gpr_sp = 13,                                /**< User stack pointer. Not the priviledged stack pointers. */
    aarch32_gpr_lr = 14,                                /**< User link register. Not the privileged link registers. */
    aarch32_gpr_pc = 15
};

/** System (non-user) registers for AArch32. */
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
    aarch32_sys_fpsid
};

} // namespace
} // namespace

#endif
#endif
