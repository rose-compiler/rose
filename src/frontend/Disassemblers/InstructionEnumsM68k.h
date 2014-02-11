#ifndef ROSE_InstructionEnumsM68k_H
#define ROSE_InstructionEnumsM68k_H

enum M68kRegisterClass {
    m68k_regclass_data,                 /**< Data registers. */
    m68k_regclass_addr,                 /**< Address registers. */
    m68k_regclass_fpr,                  /**< Floating point registers. */
    m68k_regclass_spr,                  /**< Special purpose registers. */
    m68k_regclass_mac,                  /**< Multiply-accumulate registers. */
    m68k_regclass_emac,                 /**< Extended multiply-accumulate registers. */
    m68k_regclass_sup,                  /**< Supervisor registers. */
};

enum M68kSpecialPurposeRegister {
    m68k_spr_pc,                        /**< Program counter. */
    m68k_spr_ccr,                       /**< Condition code register. */
    m68k_spr_fpcr,                      /**< Floating-point control register. */
    m68k_spr_fpsr,                      /**< Floating-point status register. */
    m68k_spr_fpiar,                     /**< Floating-point instruction address register. */
};

enum M68kMacRegister {
    m68k_mac_macsr,                     /**< MAC status register. */
    m68k_mac_acc,                       /**< MAC 32-bit accumulator. */
    m68k_mac_mask,                      /**< MAC mask register. */
};

enum M68kEmacRegister {
    m68k_emac_macsr,                     /**< EMAC status register. */
    m68k_emac_acc0,                      /**< EMAC accumulator 0. */
    m68k_emac_acc1,                      /**< EMAC accumulator 1. */
    m68k_emac_acc2,                      /**< EMAC accumulator 2. */
    m68k_emac_acc3,                      /**< EMAC accumulator 3. */
    m68k_emac_mask,                      /**< EMAC mask register. */
};

enum M68kSupervisorRegister {
    m68k_sup_sr,                        /**< Status register. */
    m68k_sup_ssp,                       /**< Supervisor stack pointer. */
    m68k_sup_vbr,                       /**< Vector base register. */
    m68k_sup_cacr,                      /**< Cache control register. */
    m68k_sup_asid,                      /**< Address space ID register. */
    m68k_sup_acr0,                      /**< Access control register 0 (data). */
    m68k_sup_acr1,                      /**< Access control register 1 (data). */
    m68k_sup_acr2,                      /**< Access control register 2 (instruction). */
    m68k_sup_acr3,                      /**< Access control register 3 (instruction). */
    m68k_sup_mmubar,                    /**< MMU base address register. */
    m68k_sup_rombar0,                   /**< ROM base address register 0. */
    m68k_sup_rombar1,                   /**< ROM base address register 1. */
    m68k_sup_rambar0,                   /**< RAM base address register 0. */
    m68k_sup_rambar1,                   /**< RAM base address register 1. */
    m68k_sup_mbar,                      /**< Module base address register. */
};

enum M68kEffectiveAddressMode {
    m68k_eam_drd   = 0x0001,            /**< Data register direct: Dn */
    m68k_eam_ard   = 0x0002,            /**< Address register direct: An */
    m68k_eam_ari   = 0x0004,            /**< Address register indirect: (An) */
    m68k_eam_inc   = 0x0008,            /**< Address register indirect with post increment: (An)+ */
    m68k_eam_dec   = 0x0010,            /**< Address register indirect with pre decrement: -(An) */
    m68k_eam_dsp   = 0x0020,            /**< Address register indirect with displacement: (d_16,An) */
    m68k_eam_idx   = 0x0040,            /**< Address register indirect with scaled index and 8-bit displacement */
    m68k_eam_pcdsp = 0x0080,            /**< Program counter indirect with displacement. */
    m68k_eam_pcidx = 0x0100,            /**< Program counter indirect with scaled index and 8-bit displacement */
    m68k_eam_absw  = 0x0200,            /**< Absolute short addressing */
    m68k_eam_absl  = 0x0400,            /**< Absolute long addression */
    m68k_eam_imm   = 0x0800,            /**< Immediate data */

    // masks
    m68k_eam_all    = 0x0fff,           /**< All addressing modes */
    m68k_eam_direct = 0x0003,           /**< Register direct addressing modes */
    m68k_eam_absolute=0x0600,           /**< Absolute addressing (word or long) */
    m68k_eam_pc     = 0x0180,           /**< Program counter indirect addressing modes */
    m68k_eam_unknown= 0x8000,           /**< Unknown addressing mode. */

    // these masks are defined in the reference manual
    m68k_eam_data   = 0x0ffd,           /**< Data addressing modes. All modes except address register direct */
    m68k_eam_memory = 0x0ffc,           /**< Memory accessing modes. All modes except register direct (includes immediate) */
    m68k_eam_control= 0x07e4,           /**< Control access modes. All modes except direct, inc, dec, and immediate. */
    m68k_eam_alter  = 0x007f,           /**< Alterable modes. From the reference manual, "alterable addressing modes refer to
                                         *   alterable (writable) operands" [ColdFire Family Programmer’s Reference Manual,
                                         *   Rev. 3, section 2.2.13]. The table indicates all modes except PC modes, absolute,
                                         *   and immediate. But there seems to be confusion in the reference manual as to what
                                         *   "alterable" means because many instructions whose text says "use only the
                                         *   alterable addressing modes from the table below" include the word and long
                                         *   absolute addressing modes in their tables. I have added comments to the various
                                         *   instructions where there is a conflict between the instruction's text and the
                                         *   instruction's table of allowed addressing modes. [Robb P. Matzke 2013-10-02] */
};

enum M68kInstructionKind {
    m68k_unknown_instruction,
    m68k_add,                           /**< Add */
    m68k_adda,                          /**< Add address */
    m68k_addi,                          /**< Add immediate */
    m68k_addq,                          /**< Add quick */
    m68k_addx,                          /**< Add extended */
    m68k_and,                           /**< AND logical */
    m68k_andi,                          /**< AND immediate */
    m68k_asl,                           /**< Arithmetic shift left */
    m68k_asr,                           /**< Arithmetic shift right */
    m68k_bcc,                           /**< Branch carry clear (alias bhs) */
    m68k_bcs,                           /**< Branch carry set (alias blo) */
    m68k_beq,                           /**< Branch equal */
    m68k_bge,                           /**< Branch greater or equal */
    m68k_bgt,                           /**< Branch greater than */
    m68k_bhi,                           /**< Branch high */
    m68k_ble,                           /**< Branch less or equal */
    m68k_bls,                           /**< Branch lower or same */
    m68k_blt,                           /**< Branch less than */
    m68k_bmi,                           /**< Branch minus */
    m68k_bne,                           /**< Branch not equal */
    m68k_bpl,                           /**< Branch plus */
    m68k_bvc,                           /**< Branch overflow clear */
    m68k_bvs,                           /**< Branch overflow set */
    m68k_bchg,                          /**< Test a bit and change */
    m68k_bclr,                          /**< Test a bit and clear */
    m68k_bitrev,                        /**< Bit reverse register */
    m68k_bra,                           /**< Branch always */
    m68k_bset,                          /**< Test a bit and set */
    m68k_bsr,                           /**< Branch to subroutine */
    m68k_btst,                          /**< Test a bit */
    m68k_byterev,                       /**< Byte reverse register */
    m68k_clr,                           /**< Clear an operand */
    m68k_cmp,                           /**< Compare */
    m68k_cmpa,                          /**< Compare address */
    m68k_cmpi,                          /**< Compare immediate */
    m68k_cpushl,
    m68k_divs,                          /**< Signed divide */
    m68k_divu,                          /**< Unsigned divide */
    m68k_eor,                           /**< Exclusive-OR logical */
    m68k_eori,                          /**< Exclusive-OR immediate */
    m68k_ext,                           /**< Sign extend */
    m68k_extb,                          /**< Sign extend byte to longword */
    m68k_fabs,
    m68k_fadd,
    m68k_fbcc,                          // FIXME: split
    m68k_fcmp,
    m68k_fdabs,
    m68k_fdadd,
    m68k_fddiv,
    m68k_fdiv,
    m68k_fdmove,
    m68k_fdmul,
    m68k_fdneg,
    m68k_fdsqrt,
    m68k_fdsub,
    m68k_ff1,                           /**< Find first one in register */
    m68k_fint,
    m68k_fintrz,
    m68k_fmove,
    m68k_fmovem,
    m68k_fmul,
    m68k_fneg,
    m68k_fnop,
    m68k_frestore,
    m68k_fsabs,
    m68k_fsadd,
    m68k_fsave,
    m68k_fsdiv,
    m68k_fsmove,
    m68k_fsmul,
    m68k_fsneg,
    m68k_fsqrt,
    m68k_fssqrt,
    m68k_fssub,
    m68k_fsub,
    m68k_ftst,
    m68k_halt,                          /**< Halt the CPU */
    m68k_illegal,                       /**< Take illegal instruction trap */
    m68k_intouch,
    m68k_jmp,                           /**< Jump */
    m68k_jsr,                           /**< Jump to subroutine */
    m68k_lea,                           /**< Load effective address */
    m68k_link,                          /**< Link and allocate */
    m68k_lsl,                           /**< Logical shift left */
    m68k_lsr,                           /**< Logical shift right */
    m68k_mac,                           /**< Multiply accumulate */
    m68k_mov3q,                         /**< Move 3-bit data quick */
    m68k_movclr,
    m68k_move,                          /**< Move from source to destination (data, CCR, ACC, MACSR, MASK) */
    m68k_movea,                         /**< Move address from source to destination */
    m68k_movec,
    m68k_movem,                         /**< Move multiple registers */
    m68k_moveq,                         /**< Move quick */
    m68k_msac,                          /**< Multiply subtract */
    m68k_muls,                          /**< Signed multiply */
    m68k_mulu,                          /**< Unsigned multiply */
    m68k_mvs,                           /**< Move with sign extend */
    m68k_mvz,                           /**< Move with zero fill */
    m68k_neg,                           /**< Negate */
    m68k_negx,                          /**< Negate with extend */
    m68k_nop,                           /**< No operation */
    m68k_not,                           /**< Logical complement */
    m68k_or,                            /**< Inclusive-OR logical */
    m68k_ori,                           /**< Inclusive-OR immediate */
    m68k_pea,                           /**< Push effective address */
    m68k_pulse,                         /**< Generate unique processor status */
    m68k_rems,                          /**< Signed divide remainder */
    m68k_remu,                          /**< Unsigned divide remainder */
    m68k_rte,                           /**< Return from exception */
    m68k_rts,                           /**< Return from subroutine */
    m68k_sats,                          /**< Signed saturate */
    m68k_st,                            /**< Set if true */
    m68k_sf,                            /**< Set if false */
    m68k_shi,                           /**< Set if high */
    m68k_sls,                           /**< Set if lower or same */
    m68k_scc,                           /**< Set if carry clear (HS) */
    m68k_scs,                           /**< Set if carry set (LO) */
    m68k_sne,                           /**< Set if not equal */
    m68k_seq,                           /**< Set if equal */
    m68k_svc,                           /**< Set if overflow clear */
    m68k_svs,                           /**< Set if overflow set */
    m68k_spl,                           /**< Set if plus */
    m68k_smi,                           /**< Set if minus */
    m68k_sge,                           /**< Set if greater or equal */
    m68k_slt,                           /**< Set if less than */
    m68k_sgt,                           /**< Set if greater than */
    m68k_sle,                           /**< Set if less or equal */
    m68k_stop,
    m68k_sub,                           /**< Subtract */
    m68k_suba,                          /**< Subtract address */
    m68k_subi,                          /**< Subtract immediate */
    m68k_subq,                          /**< Subtract quick */
    m68k_subx,                          /**< Subtract extended */
    m68k_swap,                          /**< Swap register halves */
    m68k_tas,                           /**< Test and set an operand */
    m68k_tpf,                           /**< Trap false (no operation) */
    m68k_trap,                          /**< Trap */
    m68k_tst,                           /**< Test an operand */
    m68k_unlk,                          /**< Unlink */
    m68k_wddata,                        /**< Write to debug data */
    m68k_wdebug,

    // must be last
    m68k_last_instruction
};

#endif
