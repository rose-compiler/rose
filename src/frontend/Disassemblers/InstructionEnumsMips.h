/* Enum constants for MIPS architectures */
#ifndef ROSE_BinaryAnalysis_InstructionEnumsMips_H
#define ROSE_BinaryAnalysis_InstructionEnumsMips_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {

/** MIPS major register numbers. */
enum MipsRegisterClass {
    mips_regclass_gpr,          /**< General purpose registers. Minors are 0..31. */
    mips_regclass_spr,          /**< Special purpose registers. Minors are the MipsSpecialPurposeRegisters enum members. */
    mips_regclass_fpr,          /**< Floating point registers. Minors are 0..31. */
    mips_regclass_fcsr,         /**< Floating point control/status register. Minors are MipsFcsrMinor. */
    mips_regclass_cp0gpr,       /**< General purpose registers for coprocessor 0. Minors are 0..31. */
    mips_regclass_cp2gpr,       /**< Coprocessor 2 general purpose registers. Minors are implementation defined. */
    mips_regclass_cp2spr,       /**< Coprocessor 2 special purpose (i.e., control) registers. Minors are implementation dfnd. */
    mips_regclass_sgpr          /**< Shadow general purpose registers. */
};

/** Portions of the FCSR register. These are the minor numbers for mips_regclass_fcsr. */
enum MipsFcsrMinors {
    mips_fcsr_all,              /**< All bits of the FCSR. */
    mips_fcsr_fccr,             /**< Bits that compose the FP condition codes register. */
    mips_fcsr_fexr,             /**< Bits that compose the FP exceptions register. */
    mips_fcsr_fenr              /**< Bits that compose the FP enables register. */
};

/** MIPS special purpose register minor numbers. */
enum MipsSpecialPurposeRegister {
    mips_spr_hi,                /**< Hi floating point result. */
    mips_spr_lo,                /**< Lo floating point result. */
    mips_spr_pc,                /**< Program counter. */
    mips_spr_fir,               /**< Floating point implementation and revision register. */
    mips_spr_fcsr               /**< Floating point control/status register (used to be known as FCR31) */
};

/** Kinds of MIPS instructions. */
enum MipsInstructionKind {
    mips_unknown_instruction,
    mips_abs_s,                 /**< Floating point absolute value single precision. */
    mips_abs_d,                 /**< Floating point absolute value double precision. */
    mips_abs_ps,                /**< Floating point absolute value pair of single precision. */
    mips_add,                   /**< Add word. */
    mips_add_s,                 /**< Floating point add single precision. */
    mips_add_d,                 /**< Floating point add double precision. */
    mips_add_ps,                /**< Floating point add pair of single precision. */
    mips_addi,                  /**< Add immediate word. */
    mips_addiu,                 /**< Add immediate unsigned word. */
    mips_addu,                  /**< Add unsigned word. */
    mips_alnv_ps,               /**< Floating point align variable pair of single precision. */
    mips_and,                   /**< Bitwise logical AND. */
    mips_andi,                  /**< Bitwise logical AND immediate. */
    mips_bc1f,                  /**< Branch on FP false. */
    mips_bc1fl,                 /**< Branch on FP false likely. */
    mips_bc1t,                  /**< Branch on FP true. */
    mips_bc1tl,                 /**< Branch on FP true likely. */
    mips_bc2f,                  /**< Branch on COP2 false. */
    mips_bc2fl,                 /**< Branch on COP2 false likely. */
    mips_bc2t,                  /**< Branch on COP2 true. */
    mips_bc2tl,                 /**< Branch on COP2 true likely. */
    mips_beq,                   /**< Branch on equal. */
    mips_beql,                  /**< Branch on equal likely. */
    mips_bgez,                  /**< Branch on greater than or equal to zero. */
    mips_bgezal,                /**< Branch on greater than or equal to zero and link. */
    mips_bgezall,               /**< Branch on greater than or equal to zero and link likely. */
    mips_bgezl,                 /**< Branch on greater than or equal to zero likely. */
    mips_bgtz,                  /**< Branch on greater than zero. */
    mips_bgtzl,                 /**< Branch on greater than zero likely. */
    mips_blez,                  /**< Branch on less than or equal to zero. */
    mips_blezl,                 /**< Branch on less than or equal to zero likely. */ 
    mips_bltz,                  /**< Branch on less than zero. */
    mips_bltzal,                /**< Branch on less than zero and link. */
    mips_bltzall,               /**< Branch on less than zero and link likely. */
    mips_bltzl,                 /**< Branch on less than zero likely. */
    mips_bne,                   /**< Branch on not equal. */
    mips_bnel,                  /**< Branch on not equal likely. */
    mips_break,                 /**< Break point. */
    mips_c_f_s,                 /**< Floating point compare false single precision. */
    mips_c_un_s,                /**< Floating point compare unordered single precision. */
    mips_c_eq_s,                /**< Floating point compare equal single precision. */
    mips_c_ueq_s,               /**< Floating point compare unordered equal. */
    mips_c_olt_s,               /**< Floating point compare ordered or less than single precision. */
    mips_c_ult_s,               /**< Floating point compare unordered or less than single precision. */
    mips_c_ole_s,               /**< Floating point compare ordered or less than or equal single precision. */
    mips_c_ule_s,               /**< Floating point compare unordered or less than or equal single precision. */
    mips_c_sf_s,                /**< Floating point compare signaling false single precision. */
    mips_c_ngle_s,              /**< Floating point compare not greater than or less than or equal single precision. */
    mips_c_seq_s,               /**< Floating point compare signaling equal single precision. */
    mips_c_ngl_s,               /**< Floating point compare not greater than or less than single precision. */
    mips_c_lt_s,                /**< Floating point compare less than single precision. */
    mips_c_nge_s,               /**< Floating point compare not greater than or equal single precision. */
    mips_c_le_s,                /**< Floating point compare less than or equal single precision. */
    mips_c_ngt_s,               /**< Floating point compare not greater than single precision. */
    mips_c_f_d,                 /**< Floating point compare false double precision. */
    mips_c_un_d,                /**< Floating point compare unordered double precision. */
    mips_c_eq_d,                /**< Floating point compare equal double precision. */
    mips_c_ueq_d,               /**< Floating point compare unordered equal. */
    mips_c_olt_d,               /**< Floating point compare ordered or less than double precision. */
    mips_c_ult_d,               /**< Floating point compare unordered or less than double precision. */
    mips_c_ole_d,               /**< Floating point compare ordered or less than or equal double precision. */
    mips_c_ule_d,               /**< Floating point compare unordered or less than or equal double precision. */
    mips_c_sf_d,                /**< Floating point compare signaling false double precision. */
    mips_c_ngle_d,              /**< Floating point compare not greater than or less than or equal double precision. */
    mips_c_seq_d,               /**< Floating point compare signaling equal double precision. */
    mips_c_ngl_d,               /**< Floating point compare not greater than or less than double precision. */
    mips_c_lt_d,                /**< Floating point compare less than double precision. */
    mips_c_nge_d,               /**< Floating point compare not greater than or equal double precision. */
    mips_c_le_d,                /**< Floating point compare less than or equal double precision. */
    mips_c_ngt_d,               /**< Floating point compare not greater than double precision. */
    mips_c_f_ps,                /**< Floating point compare false pair of single precision. */
    mips_c_un_ps,               /**< Floating point compare unordered pair of single precision. */
    mips_c_eq_ps,               /**< Floating point compare equal pair of single precision. */
    mips_c_ueq_ps,              /**< Floating point compare unordered equal. */
    mips_c_olt_ps,              /**< Floating point compare ordered or less than pair of single precision. */
    mips_c_ult_ps,              /**< Floating point compare unordered or less than pair of single precision. */
    mips_c_ole_ps,              /**< Floating point compare ordered or less than or equal pair of single precision. */
    mips_c_ule_ps,              /**< Floating point compare unordered or less than or equal pair of single precision. */
    mips_c_sf_ps,               /**< Floating point compare signaling false pair of single precision. */
    mips_c_ngle_ps,             /**< Floating point compare not greater than or less than or equal pair of single precision. */
    mips_c_seq_ps,              /**< Floating point compare signaling equal pair of single precision. */
    mips_c_ngl_ps,              /**< Floating point compare not greater than or less than pair of single precision. */
    mips_c_lt_ps,               /**< Floating point compare less than pair of single precision. */
    mips_c_nge_ps,              /**< Floating point compare not greater than or equal pair of single precision. */
    mips_c_le_ps,               /**< Floating point compare less than or equal pair of single precision. */
    mips_c_ngt_ps,              /**< Floating point compare not greater than pair of single precision. */
    mips_cache,                 /**< Perform cache operation. */
    mips_cachee,                /**< Perform cache operation EVA. */
    mips_ceil_l_s,              /**< Fixed point ceiling convert to long fixed point. */
    mips_ceil_l_d,              /**< Fixed point ceiling convert to long fixed point. */
    mips_ceil_w_s,              /**< Fixed point ceiling convert to word fixed point. */
    mips_ceil_w_d,              /**< Fixed point ceiling convert to word fixed point. */
    mips_cfc1,                  /**< Move control word from floating point. */
    mips_cfc2,                  /**< Move control word from coprocessor 2. */
    mips_clo,                   /**< Count leading ones in word. */
    mips_clz,                   /**< Count leading zeros in word. */
    mips_cop2,                  /**< Coprocessor operation to coprocessor 2. */
    mips_ctc1,                  /**< Move control word to floating point. */
    mips_ctc2,                  /**< Move control word to coprocessor 2. */
    mips_cvt_d_s,               /**< Floating point convert to double floating point. */
    mips_cvt_d_w,               /**< Floating point convert to double floating point. */
    mips_cvt_d_l,               /**< Floating point convert to double floating point. */
    mips_cvt_l_s,               /**< Floating point convert to long fixed point. */
    mips_cvt_l_d,               /**< Floating point convert to long fixed point. */
    mips_cvt_ps_s,              /**< Floating point convert to paired single. */
    mips_cvt_s_d,               /**< Floating point convert to single floating point. */
    mips_cvt_s_w,               /**< Floating point convert to single floating point. */
    mips_cvt_s_l,               /**< Floating point convert to single floating point. */
    mips_cvt_s_pl,              /**< Floating point convert pair lower to single floating point. */
    mips_cvt_s_pu,              /**< Floating point convert pair upper to single floating point. */
    mips_cvt_w_s,               /**< Floating point convert to word fixed point. */
    mips_cvt_w_d,               /**< Floating point convert to word fixed point. */
    mips_di,                    /**< Disable interrupts. */
    mips_div,                   /**< Divide word. */
    mips_div_s,                 /**< Floating point divide. */
    mips_div_d,                 /**< Floating point divide. */
    mips_divu,                  /**< Divide unsigned word. */
    mips_ehb,                   /**< Execution hazard barrier. */
    mips_ei,                    /**< Enable interrupts. */
    mips_eret,                  /**< Exception return. */
    mips_ext,                   /**< Extract bit field. */
    mips_floor_l_s,             /**< Floating point floor convert to long fixed point. */
    mips_floor_l_d,             /**< Floating point floor convert to long fixed point. */
    mips_floor_w_s,             /**< Floating point floor convert to word fixed point. */
    mips_floor_w_d,             /**< Floating point floor convert to word fixed point. */
    mips_ins,                   /**< Insert bit field. */
    mips_j,                     /**< Jump. */
    mips_jal,                   /**< Jump and link. */
    mips_jalr,                  /**< Jump and link register. */
    mips_jalr_hb,               /**< Jump and link register with hazard barrier. */
    mips_jalx,                  /**< Jump and link exchange. */
    mips_jr,                    /**< Jump register. */
    mips_jr_hb,                 /**< Jump register with hazard barrier. */
    mips_lb,                    /**< Load byte. */
    mips_lbe,                   /**< Load byte EVA. */
    mips_lbu,                   /**< Load byte unsigned. */
    mips_lbue,                  /**< Load byte unsigned EVA. */
    mips_ldc1,                  /**< Load doubleword to floating point. */
    mips_ldc2,                  /**< Load doubleword to coprocessor 2. */
    mips_ldxc1,                 /**< Load doubleword indexed to floating point. */
    mips_lh,                    /**< Load halfword. */
    mips_lhe,                   /**< Load halfword EVA. */
    mips_lhu,                   /**< Load halfword unsigned. */
    mips_lhue,                  /**< Load halfword unsigned EVA. */
    mips_ll,                    /**< Load linked word. */
    mips_lle,                   /**< Load linked word EVA. */
    mips_lui,                   /**< Load upper immediate. */
    mips_luxc1,                 /**< Load doubleword indexed unaligned to floating point. */
    mips_lw,                    /**< Load word. */
    mips_lwc1,                  /**< Load word to floating point. */
    mips_lwc2,                  /**< Load word to coprocessor 2. */
    mips_lwe,                   /**< Load word EVA. */
    mips_lwl,                   /**< Load word left. */
    mips_lwle,                  /**< Load word left EVA. */
    mips_lwr,                   /**< Load word right. */
    mips_lwre,                  /**< Load word right EVA. */
    mips_lwxc1,                 /**< Load word indexed to floating point. */
    mips_madd,                  /**< Multiply and add word to hi, lo. */
    mips_madd_s,                /**< Floating point multiply add. */
    mips_madd_d,                /**< Floating point multiply add. */
    mips_madd_ps,               /**< Floating point multiply add. */
    mips_maddu,                 /**< Multiply and add unsigned word to hi, lo. */
    mips_mfc0,                  /**< Move from coprocessor 0. */
    mips_mfc1,                  /**< Move word from coprocessor 1. */
    mips_mfc2,                  /**< Move word from coprocessor 2. */
    mips_mfhc1,                 /**< Move word from high half of floating point register. */
    mips_mfhc2,                 /**< Move word from high half of coprocessor 2. */
    mips_mfhi,                  /**< Move from hi register. */
    mips_mflo,                  /**< Move from lo register. */
    mips_mov_s,                 /**< Floating point move. */
    mips_mov_d,                 /**< Floating point move. */
    mips_mov_ps,                /**< Floating point move. */
    mips_movf,                  /**< Move conditional on floating point false. */
    mips_movf_s,                /**< Floating point move conditional on floating point false. */
    mips_movf_d,                /**< Floating point move conditional on floating point false. */
    mips_movf_ps,               /**< Floating point move conditional on floating point false. */
    mips_movn,                  /**< Move conditional on not zero. */
    mips_movn_s,                /**< Move floating point conditional on not zero. */
    mips_movn_d,                /**< Move floating point conditional on not zero. */
    mips_movn_ps,               /**< Move floating point conditional on not zero. */
    mips_movt,                  /**< Move conditional on floating piont true. */
    mips_movt_s,                /**< Floating point move conditional on floating point true. */
    mips_movt_d,                /**< Floating point move conditional on floating point true. */
    mips_movt_ps,               /**< Floating point move conditional on floating point true. */
    mips_movz,                  /**< Move conditional on zero. */
    mips_movz_s,                /**< Floating point move conditional on zero. */
    mips_movz_d,                /**< Floating point move conditional on zero. */
    mips_movz_ps,               /**< Floating point move conditional on zero. */
    mips_msub,                  /**< Multiply and subtract word. */
    mips_msub_s,                /**< Floating point multiple and subtract. */
    mips_msub_d,                /**< Floating point multiple and subtract. */
    mips_msub_ps,               /**< Floating point multiple and subtract. */
    mips_msubu,                 /**< Multiply and subtract word to hi, lo. */
    mips_mtc0,                  /**< Move to coprocessor 0. */
    mips_mtc1,                  /**< Move word to floating point. */
    mips_mtc2,                  /**< Move word to coprocessor 2. */
    mips_mthc1,                 /**< Move word to high half of floating point register. */
    mips_mthc2,                 /**< Move word to high half of coprocessor 2 register. */
    mips_mthi,                  /**< Move to hi register. */
    mips_mtlo,                  /**< Move to lo register. */
    mips_mul,                   /**< Multiply word to GPR. */
    mips_mul_s,                 /**< Floating point multiply. */
    mips_mul_d,                 /**< Floating point multiply. */
    mips_mul_ps,                /**< Floating point multiply. */
    mips_mult,                  /**< Multiply word. */
    mips_multu,                 /**< Multiply unsigned word. */
    mips_neg_s,                 /**< Floating point negate. */
    mips_neg_d,                 /**< Floating point negate. */
    mips_neg_ps,                /**< Floating point negate. */
    mips_nmadd_s,               /**< Floating point negative multiply add. */
    mips_nmadd_d,               /**< Floating point negative multiply add. */
    mips_nmadd_ps,              /**< Floating point negative multiply add. */
    mips_nmsub_s,               /**< Floating point negative multply subtract. */
    mips_nmsub_d,               /**< Floating point negative multply subtract. */
    mips_nmsub_ps,              /**< Floating point negative multply subtract. */
    mips_nop,                   /**< No operation. */
    mips_nor,                   /**< Not OR. */
    mips_or,                    /**< Bitwise OR. */
    mips_ori,                   /**< Bitwise OR immediate. */
    mips_pause,                 /**< Wait for the LLBit to clear. */
    mips_pll_ps,                /**< Pair lower lower. */
    mips_plu_ps,                /**< Pair lower upper. */
    mips_pref,                  /**< Prefetch. */
    mips_prefe,                 /**< Prefetch EVA. */
    mips_prefx,                 /**< Prefetch indexed. */
    mips_pul_ps,                /**< Pair upper lower. */
    mips_puu_ps,                /**< Pair upper upper. */
    mips_rdhwr,                 /**< Read hardware register. */
    mips_rdpgpr,                /**< Read GPR from previous shadow set. */
    mips_recip_s,               /**< Reciprocal approximation. */
    mips_recip_d,               /**< Reciprocal approximation. */
    mips_rotr,                  /**< Rotate word right. */
    mips_rotrv,                 /**< Rotate word right variable. */
    mips_round_l_s,             /**< Floating point round to long fixed point. */
    mips_round_l_d,             /**< Floating point round to long fixed point. */
    mips_round_w_s,             /**< Floating point round to word fixed point. */
    mips_round_w_d,             /**< Floating point round to word fixed point. */
    mips_rsqrt_s,               /**< Reciprocal square root approximation. */
    mips_rsqrt_d,               /**< Reciprocal square root approximation. */
    mips_sb,                    /**< Store byte. */
    mips_sbe,                   /**< Store byte EVA. */
    mips_sc,                    /**< Store conditional word. */
    mips_sce,                   /**< Store conditional word EVA. */
    mips_sdc1,                  /**< Store doubleword from floating point. */
    mips_sdc2,                  /**< Store doubleword from coprocessor 2. */
    mips_sdxc1,                 /**< Store doubleword indexed from floating point. */
    mips_seb,                   /**< Sign extend byte. */
    mips_seh,                   /**< Sign extend halfword. */
    mips_sh,                    /**< Store halfword. */
    mips_she,                   /**< Store halfword EVA. */
    mips_sll,                   /**< Shift word left logical. */
    mips_sllv,                  /**< Shift word left logical variable. */
    mips_slt,                   /**< Set on less than. */
    mips_slti,                  /**< Set on less than immediate. */
    mips_sltiu,                 /**< Set on less than immediate unsigned. */
    mips_sltu,                  /**< Set on less than unsigned. */
    mips_sqrt_s,                /**< Floating point square root. */
    mips_sqrt_d,                /**< Floating point square root. */
    mips_sra,                   /**< Shift word right arithmetic. */
    mips_srav,                  /**< Shift word right arithmetic variable. */
    mips_srl,                   /**< Shift right logical. */
    mips_srlv,                  /**< Shift right logical value. */
    mips_ssnop,                 /**< Superscalar no operation. */
    mips_sub,                   /**< Subtract word. */
    mips_sub_s,                 /**< Subtract floating point. */
    mips_sub_d,                 /**< Subtract floating point. */
    mips_sub_ps,                /**< Subtract floating point. */
    mips_subu,                  /**< Subtract unsigned word. */
    mips_suxc1,                 /**< Store doubleword indexed unaligned from floating point. */
    mips_sw,                    /**< Store word. */
    mips_swc1,                  /**< Store word from floating point. */
    mips_swc2,                  /**< Store word from coprocessor 2. */
    mips_swe,                   /**< Store word EVA. */
    mips_swl,                   /**< Store word left. */
    mips_swle,                  /**< Store word left EVA. */
    mips_swr,                   /**< Store word right. */
    mips_swre,                  /**< Store word right EVA. */
    mips_swxc1,                 /**< Store word indexed from floating point. */
    mips_sync,                  /**< Synchronize. */
    mips_synci,                 /**< Synchronize caches to make instruction writes effective. */
    mips_syscall,               /**< System call. */
    mips_teq,                   /**< Trap if equal. */
    mips_teqi,                  /**< Trap if equal immediate. */
    mips_tge,                   /**< Trap if greater or equal. */
    mips_tgei,                  /**< Trap if greater or equal immediate. */
    mips_tgeiu,                 /**< Trap if greater or equal immediate unsigned. */
    mips_tgeu,                  /**< Trap if greater or equal unsigned. */
    mips_tlbinv,                /**< TLB invalidate. */
    mips_tlbinvf,               /**< TLB invalidate flush. */
    mips_tlbp,                  /**< Probe TLB for matching entry. */
    mips_tlbr,                  /**< Read indexed TLB entry. */
    mips_tlbwi,                 /**< Write indexed TLB entry. */
    mips_tlbwr,                 /**< Write random TLB entry. */
    mips_tlt,                   /**< Trap if less than. */
    mips_tlti,                  /**< Trap if less than immediate. */
    mips_tltiu,                 /**< Trap if less than immediate unsigned. */
    mips_tltu,                  /**< Trap if less than unsigned. */
    mips_tne,                   /**< Trap if not equal. */
    mips_tnei,                  /**< Trap if not equal immediate. */
    mips_trunc_l_s,             /**< Floating point truncate to long fixed point. */
    mips_trunc_l_d,             /**< Floating point truncate to long fixed point. */
    mips_trunc_w_s,             /**< Floating point truncate to word fixed point. */
    mips_trunc_w_d,             /**< Floating point truncate to word fixed point. */
    mips_wait,                  /**< Enter standby mode. */
    mips_wrpgpr,                /**< Write to GPR in previous shadow set. */
    mips_wsbh,                  /**< Word swap bytes within halfwords. */
    mips_xor,                   /**< Exclusive OR. */
    mips_xori,                  /**< Exclusive OR immediate. */

    mips_last_instruction // must be last enum member
};

} // namespace
} // namespace

#endif
#endif
