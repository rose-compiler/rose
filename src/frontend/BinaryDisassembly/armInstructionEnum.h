#ifndef ROSE_ARMINSTRUCTIONENUM_H
#define ROSE_ARMINSTRUCTIONENUM_H

#include <string>

enum ArmInstructionKind {
  arm_unknown_instruction = 0,
  arm_adc,                              /**< add with carry */
  arm_adcs,                             /**< add with carry and update condition word */
  arm_add,                              /**< add */
  arm_adds,                             /**< add and update condition word */
  arm_and,                              /**< bitwise AND */
  arm_ands,                             /**< bitwise AND and update condition word */
  arm_b,                                /**< branch */
  arm_bic,                              /**< bit clear (op1 AND NOT op2) */
  arm_bics,                             /**< bit clear and update condition word */
  arm_bkpt,                             /**< breakpoint */
  arm_bl,                               /**< branch with link */
  arm_blx,                              /**< branch with link and exchange */
  arm_bx,                               /**< branch and exchange */
  arm_bxj,                              /**< branch and change to Java state */
  arm_clz,                              /**< count leading zeros */
  arm_cmn,                              /**< compare negative */
  arm_cmp,                              /**< compare */
  arm_eor,                              /**< bitwise exclusive-OR */
  arm_eors,                             /**< bitwise exclusive-EOR and update condition word */
  arm_ldm,                              /**< pop or block data load */
  arm_ldmda,                            /**< pop or block data load, decrement after */
  arm_ldmdb,                            /**< pop or block data load, decrement before */
  arm_ldmia,                            /**< pop or block data load, increment after */
  arm_ldmib,                            /**< pop or block data load, increment before */
  arm_ldr,                              /**< load word */
  arm_ldrb,                             /**< load byte */
  arm_ldrbt,                            /**< load byte user mode privilege */
  arm_ldrd,                             /**< load doubleword */
  arm_ldrsb,                            /**< load signed byte */
  arm_ldrsh,                            /**< load signed halfword */
  arm_ldrt,                             /**< load word user mode privilege */
  arm_ldruh,    /*synonym for ldrh?*/
  arm_mla,                              /**< multiply and accumulate */
  arm_mlas,                             /**< multiply and accumulate and update condition word */
  arm_mov,                              /**< move */
  arm_movs,                             /**< move and update condition word */
  arm_mrs,                              /**< move PSR to register */
  arm_msr,                              /**< move register to PSR */
  arm_mul,                              /**< multiply */
  arm_muls,                             /**< multiply and update condition word */
  arm_mvn,                              /**< move NOT */
  arm_mvns,                             /**< move NOT and update condition word */
  arm_orr,                              /**< bitwise OR */
  arm_orrs,                             /**< bitwise OR and update condition word */
  arm_qadd,                             /**< saturating add */
  arm_qdadd,                            /**< double saturating add */
  arm_qdsub,                            /**< double saturating subtract */
  arm_qsub,                             /**< saturating subtract */
  arm_rsb,                              /**< reverse subtract */
  arm_rsbs,                             /**< reverse subtract and update condition word */
  arm_rsc,                              /**< reverse subtract with carry */
  arm_rscs,                             /**< reverse subtract with carry and update condition word */
  arm_sbc,                              /**< subtract with carry */
  arm_sbcs,                             /**< subtract with carry and update condition word */
  arm_setend,                           /**< set endianness */
  arm_smlabb,                           /**< signed multiply long and accumulate 16 * 16 bit */
  arm_smlabt,                           /**< signed multiply long and accumulate 16 * 16 bit */
  arm_smlal,                            /**< signed multiply long and accumulate long */
  arm_smlalbb,                          /**< signed multiply long 16 * 16 bit and accumulate long */
  arm_smlalbt,                          /**< signed multiply long 16 * 16 bit and accumulate long */
  arm_smlals,                           /**< signed multiply long and accumulate long and update condition word */
  arm_smlaltb,                          /**< signed multiply long 16 * 16 bit and accumulate long */
  arm_smlaltt,                          /**< signed multiply long 16 * 16 bit and accumulate long */
  arm_smlatb,                           /**< signed multiply long and accumulate 16 * 16 bit */
  arm_smlatt,                           /**< signed multiply long and accumulate 16 * 16 bit */
  arm_smlawb,                           /**< signed multiply long and accumulate 32 * 16 bit */
  arm_smlawt,                           /**< signed multiply long and accumulate 32 * 16 bit */
  arm_smluwb,   /*should this be smulwb?*/
  arm_smluwt,   /*should this be smulwt?*/
  arm_smulbb,                           /**< signed multiply long 16 * 16 bit */
  arm_smulbt,                           /**< signed multiply long 16 * 16 bit */
  arm_smull,                            /**< signed multiply long */
  arm_smulls,                           /**< signed multiply long and update condition word */
  arm_smultb,                           /**< signed multiply long 16 * 16 bit */
  arm_smultt,                           /**< signed multiply long 16 * 16 bit */
  arm_stm,                              /**< push or block data store */
  arm_stmda,                            /**< push or block data store, decrement after */
  arm_stmdb,                            /**< push or block data store, decrement before */
  arm_stmia,                            /**< push or block data store, increment after */
  arm_stmib,                            /**< push or block data store, increment before */
  arm_str,                              /**< store word */
  arm_strb,                             /**< store byte */
  arm_strbt,                            /**< store byte user mode privilege */
  arm_strd,                             /**< store doubleword */
  arm_strh,                             /**< store halfword */
  arm_strt,                             /**< store word user mode privilege */
  arm_sub,                              /**< subtract */
  arm_subs,                             /**< subtract and update condition word */
  arm_swi,                              /**< software interrupt */
  arm_swp,                              /**< swap word */
  arm_teq,                              /**< test equivalence */
  arm_tst,                              /**< test */
  arm_umaal,                            /**< multiply double accumulate long */
  arm_umlal,                            /**< multiply unsigned accumulate long */
  arm_umlals,                           /**< multiply unsigned accumulate long and update condition word */
  arm_umull,                            /**< multiply unsigned long */
  arm_umulls,                           /**< multiply unsigned long and update condition word */
  arm_undefined,
  arm_last_instruction
};

// These are one more than the ARM condition codes in the instruction encoding, so the order is important
enum ArmInstructionCondition {
  arm_cond_unknown = 0,
  arm_cond_eq = 1,                      /**< equal */
  arm_cond_ne = 2,                      /**< not equal */
  arm_cond_hs = 3,                      /**< carry set/unsigned higher or same (greater than or equal, or unordered) */
  arm_cond_lo = 4,                      /**< carry clear/unsigned lower (less than) */
  arm_cond_mi = 5,                      /**< negative (less than) */
  arm_cond_pl = 6,                      /**< positive or zero (greater than or equal, or unordered) */
  arm_cond_vs = 7,                      /**< overflow (unordered [at least one NaN operand) */
  arm_cond_vc = 8,                      /**< no overflow (not unordered) */
  arm_cond_hi = 9,                      /**< unsigned higher (greater than, or unordered) */
  arm_cond_ls = 10,                     /**< unsigned lower or same (less than or equal) */
  arm_cond_ge = 11,                     /**< signed greater than or equal (greater than or equal) */
  arm_cond_lt = 12,                     /**< signed less than (less than, or unordered) */
  arm_cond_gt = 13,                     /**< signed greater than (greater than) */
  arm_cond_le = 14,                     /**< signed less than or equal (less than or equal, or unordered) */
  arm_cond_al = 15,                     /**< always (normally omitted) */
  arm_cond_nv = 16                      /*??*/
};

std::string toString(ArmInstructionKind k);

#endif /* ROSE_ARMINSTRUCTIONENUM_H */
