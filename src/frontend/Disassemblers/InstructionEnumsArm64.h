#ifndef ROSE_BinaryAnalysis_InstructionEnumsArm_H
#define ROSE_BinaryAnalysis_InstructionEnumsArm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <capstone/arm64.h>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

using ::arm64_insn;
using Arm64InstructionKind = ::arm64_insn;           /**< AArch64 instruction types. */

using ::arm64_cc;
using Arm64InstructionCondition = ::arm64_cc;        /**< AArch64 condition codes. */

using ::arm64_extender;
using Arm64Extender = ::arm64_extender;              /**< AArch64 operand extenders. */

using ::arm64_vas;
using Arm64VectorArrangement = ::arm64_vas;          /**< AArch64 vector arrangement specifier. */

using ::arm64_at_op;
using Arm64AtOperation = ::arm64_at_op;              /**< AArch64 AT instruction operand. */

using ::arm64_prefetch_op;
using Arm64PrefetchOperation = ::arm64_prefetch_op;  /**< AArch64 operand for prefetch instructions. */

/** Major register numbers for AArch64. */
enum Arm64RegisterClass {
    arm_regclass_gpr,                                 // general purpose registers
    arm_regclass_sp,                                  // stack pointer registers
    arm_regclass_ext,                                 // SIMD and FP registers, so-called "extension" registers
    arm_regclass_pc,                                  // program counter, instruction pointer
    arm_regclass_cc,                                  // condition codes registers
    arm_regclass_system,                              // system registers
};

/** System registers.
 *
 * There are at most four copies of each system register, one per exception level. Since ROSE allows only 16 distinct major
 * numbers for the registers, but 1024 minor numbers, we use a single major number for all the system registers and use the
 * minor numbers for the different system registers.  For simplicity, we reserve four minor numbers for each type of system
 * register, although this might change in the future. */
enum Arm64SystemRegisters {
    arm_system_actlr     = 0,                         // auxiliary control registers
    arm_system_ccsidr    = 4,                         // current cache size ID registers
    arm_system_clidr     = 8,                         // cache level ID registers
    arm_system_cntfrq    = 12,                        // counter-timer frequency registers
    arm_system_cntpct    = 16,                        // counter-timer physical count registers
    arm_system_cntkctl   = 20,                        // counter-timer kernel control registers
    arm_system_cntp_cval = 24,                        // counter-timer physical timer compare registers
    arm_system_cpacr     = 28,                        // coprocessor access control registers
    arm_system_csselr    = 32,                        // cache size selection registers
    arm_system_cntp_ctl  = 36,                        // counter-timer physical control registers
    arm_system_ctr       = 40,                        // cache type registers
    arm_system_dczid     = 44,                        // data cache zero ID registers
    arm_system_elr       = 48,                        // exception link registers
    arm_system_esr       = 52,                        // exception syndrome registers
    arm_system_far       = 56,                        // fault address registers
    arm_system_hcr       = 60,                        // hypervisor configuration registers
    arm_system_mair      = 64,                        // memory attribute indirection registers
    arm_system_midr      = 68,                        // main ID registers
    arm_system_mpidr     = 72,                        // multiprocessor affinity registers
    arm_system_scr       = 76,                        // secure configuration registers
    arm_system_sctlr     = 80,                        // system control registers
    arm_system_spsr      = 84,                        // saved program status registers
    arm_system_tcr       = 88,                        // translation control registers
    arm_system_tpidr     = 92,                        // user read/write thread ID registers
    arm_system_tpidrr0   = 96,                        // user read-only thread ID registers
    arm_system_ttbr0     = 100,                       // translation table base registers 0
    arm_system_ttbr1     = 104,                       // translation table base register 1
    arm_system_vbar      = 108,                       // vector based address registers
    arm_system_vtcr      = 112,                       // virtualization translation control registers
    arm_system_vttbr     = 116                        // virtualization translation table base registers
};

} // namespace
} // namespace

#endif
#endif
