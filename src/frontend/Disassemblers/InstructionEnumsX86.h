/* Enum constants for Intel x86 architectures */

#ifndef ROSE_BinaryAnalysis_InstructionEnumsX86_H
#define ROSE_BinaryAnalysis_InstructionEnumsX86_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "AssemblerX86Init.h"   /* A big enum whose members are all possible x86 instructions. */

namespace Rose {
namespace BinaryAnalysis {

/** Intel x86 instruction size constants. */
enum X86InstructionSize {
    x86_insnsize_none,
    x86_insnsize_16,                                    /**< Instruction is for a 16-bit architecture. */
    x86_insnsize_32,                                    /**< Instruction is for a 32-bit architecture. */
    x86_insnsize_64                                     /**< Instruction is for a 64-bit architecture. */
};

/** Intel x86 major register numbers */
enum X86RegisterClass {
    x86_regclass_gpr,                                   /**< Minors are X86GeneralPurposeRegister
                                                         *   (ax,cx,dx,bx,sp,bp,si,di,r8..r15) */
    x86_regclass_segment,                               /**< Minors are X86SegmentRegister (es,cs,ss,ds,fs,gs) */
    x86_regclass_cr,                                    /**< Control registers; Minors are 0-4, 8 */
    x86_regclass_dr,                                    /**< Debug registers; Minors are 0-7 */
    x86_regclass_st,                                    /**< Floating point stack or MM registers; Minors are 0-7 */
    x86_regclass_xmm,                                   /**< 128-bit xmmN; Minors are 0-7. */
    x86_regclass_ip,                                    /**< Instruction pointer; Only allowed minor is zero. */
    x86_regclass_flags                                  /**< Status flags. */
};

/** Intel x86 segment registers. */
enum X86SegmentRegister {
    x86_segreg_es       = 0,                            // Numbering is based on Intel documentation
    x86_segreg_cs       = 1,
    x86_segreg_ss       = 2,
    x86_segreg_ds       = 3,
    x86_segreg_fs       = 4,
    x86_segreg_gs       = 5,
    x86_segreg_none = 16 /* For unspecified segment overrides */
};

/** Intel x86 general purpose registers */
enum X86GeneralPurposeRegister {
    x86_gpr_ax          = 0,                            // Numbering is based on Intel documentation
    x86_gpr_cx          = 1,
    x86_gpr_dx          = 2,
    x86_gpr_bx          = 3,
    x86_gpr_sp          = 4,
    x86_gpr_bp          = 5,
    x86_gpr_si          = 6,
    x86_gpr_di          = 7,
    x86_gpr_r8          = 8,
    x86_gpr_r9          = 9,
    x86_gpr_r10         = 10,
    x86_gpr_r11         = 11,
    x86_gpr_r12         = 12,
    x86_gpr_r13         = 13,
    x86_gpr_r14         = 14,
    x86_gpr_r15         = 15
};

/** Intel x86 ST-related registers.  These are the 8 80-bit floating point registers. */
enum X86StRegister {
    x86_st_0            = 0,
    x86_st_1            = 1,
    x86_st_2            = 2,
    x86_st_3            = 3,
    x86_st_4            = 4,
    x86_st_5            = 5,
    x86_st_6            = 6,
    x86_st_7            = 7,
    x86_st_nregs        = 8                             // number of ST registers
};

/** Minor numbers for x86_regclass_flag. */
enum X86Flags {
    x86_flags_status     = 0,                           // general-purpose status flags
    x86_flags_fpstatus   = 1,                           // floating-point status flags
    x86_flags_fptag      = 2,                           // floating-point tag register
    x86_flags_fpctl      = 3,                           // floating-point control register
    x86_flags_mxcsr      = 4                            // SSE control and status register
};

/** Intel x86 status flags. These are the bit offsets in the x86_flags_status register. */
enum X86Flag {
    x86_flag_cf         = 0,
    x86_flag_pf         = 2,
    x86_flag_af         = 4,
    x86_flag_zf         = 6,
    x86_flag_sf         = 7,
    x86_flag_tf         = 8,
    x86_flag_if         = 9,
    x86_flag_df         = 10,
    x86_flag_of         = 11,
    x86_flag_iopl       = 12,   /* 2 bits, 12 and 13 */
    x86_flag_nt         = 14,
    x86_flag_rf         = 16,
    x86_flag_vm         = 17,
    x86_flag_ac         = 18,
    x86_flag_vif        = 19,
    x86_flag_vip        = 20,
    x86_flag_id         = 21
};

/** Intel x86 branch prediction types. */
enum X86BranchPrediction 
{
    x86_branch_prediction_none,
    x86_branch_prediction_taken,
    x86_branch_prediction_not_taken
};

/** Intel x86 instruction repeat prefix. */
enum X86RepeatPrefix 
{
    x86_repeat_none,                                    /**< No repeat prefix */
    x86_repeat_repne,                                   /**< Repeat not equal prefix 0xf2 */
    x86_repeat_repe                                     /**< Repeat equal prefix 0xf3 */
};

/** Protected mode exceptions. These strange names come directly from the Intel documentation, section 3.1.1.11 in the
 * Instruction Set Reference (except for x86_exception_int, which ROSE uses internally for the INT instruction. */
enum X86Exception {
    x86_exception_int,                                  /**< INT instruction. minor is the imm8 argument. */
    x86_exception_sysenter,                             /**< SYSENTER instruction. */
    x86_exception_syscall,                              /**< SYSCALL instruction. */
    x86_exception_de,                                   /**< Divide error. DIV and IDIV instructions. */
    x86_exception_db,                                   /**< Debug. Any code or data reference. */
    x86_exception_bp,                                   /**< Breakpoint. INT 3 instruction. */
    x86_exception_of,                                   /**< Overflow. INTO instruction. */
    x86_exception_br,                                   /**< BOUND range exceeded. BOUND instruction. */
    x86_exception_ud,                                   /**< Invalid opcode. UD2 insn or reserved opcode. */
    x86_exception_nm,                                   /**< Device not available (no math coproc). Floating-point or
                                                         *   WAIT/FWAIT insn. */
    x86_exception_df,                                   /**< Double fault. Any insn that can generate an exception, or NMI,
                                                         *   INTR instruction.*/
    x86_exception_ts,                                   /**< Invalid TSS. Task switch or TSS access. */
    x86_exception_np,                                   /**< Segment not present. Loading segment regs or accessing system
                                                         *   segments. */
    x86_exception_ss,                                   /**< Stack segment fault. Stack operations and SS register loads. */
    x86_exception_gp,                                   /**< General protection. Any memory reference and other protection
                                                         *   checks. */
    x86_exception_pf,                                   /**< Page fault. Any memory reference. */
    x86_exception_mf,                                   /**< Floating point error (math fault). Floating-point or WAIT/FWAIT
                                                         *   instruction. */
    x86_exception_ac,                                   /**< Alignment check. Any data reference in memory. */
    x86_exception_mc,                                   /**< Machine check. Model-dependent machine check errors. */
    x86_exception_xm                                    /**< SIMD floating-point numeric error. SSE/SSE2/SSE3 floating-point
                                                         *   instructions. */
};

} // namespace
} // namespace

#endif
#endif
