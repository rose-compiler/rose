#ifndef ROSE_BinaryAnalysis_InstructionEnumsAarch32_H
#define ROSE_BinaryAnalysis_InstructionEnumsAarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <capstone/arm.h>

namespace Rose {
namespace BinaryAnalysis {

using ::arm_insn;                                       // from capstone
using Aarch32InstructionKind = ::arm_insn;              /**< ARM A32 and T32 instruction types. */

/** Major register numbers for AArch32. */
enum Aarch32RegisterClass {
    aarch32_regclass_gpr,                               /**< General purpose registers. */
    aarch32_regclass_pc,                                /**< Program counter, instruction pointer. */
    aarch32_regclass_sys                                /**< Special purpose, non-user registers. */
};

/** General purpose registers for AArch32. */
enum AArch32GeneralPurposeRegisters {
    aarch32_gpr_sp = 13,                                /**< User stack pointer. Not the priviledged stack pointers. */
    aarch32_gpr_lr = 14                                 /**< User link register. Not the privileged link registers. */
};

/** System (non-user) registers for AArch32. */
enum AArch32SystemRegisters {
    aarch32_sys_sp_hyp,
    aarch32_sys_sp_svc,
    aarch32_sys_sp_abt,
    aarch32_sys_sp_und,
    aarch32_sys_sp_mon,
    aarch32_sys_sp_irq,
    aarch32_sys_sp_fiq,
    aarch32_sys_lr_svc,
    aarch32_sys_lr_abt,
    aarch32_sys_lr_und,
    aarch32_sys_lr_mon,
    aarch32_sys_lr_irq,
    aarch32_sys_lr_fiq,
    aarch32_sys_apsr,                                   // aka., cpsr
    aarch32_sys_spsr_hyp,
    aarch32_sys_spsr_svc,
    aarch32_sys_spsr_abt,
    aarch32_sys_spsr_und,
    aarch32_sys_spsr_mon,
    aarch32_sys_spsr_irq,
    aarch32_sys_spsr_fiq
};

} // namespace
} // namespace

#endif
#endif
