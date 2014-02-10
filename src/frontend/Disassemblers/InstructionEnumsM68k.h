#ifndef ROSE_InstructionEnumsM68k_H
#define ROSE_InstructionEnumsM68k_H

enum M68kRegisterClass {
    m68k_regclass_data,                 /**< Data registers. */
    m68k_regclass_addr,                 /**< Address registers. */
    m68k_regclass_fpr,                  /**< Floating point registers. */
    m68k_regclass_spr,                  /**< Special purpose registers. */
    m68k_regclass_mac,                  /**< Multiply-accumulate registers. */
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
    m68k_mac_acc0,                      /**< MAC accumulator 0. */
    m68k_mac_acc1,                      /**< MAC accumulator 1. */
    m68k_mac_acc2,                      /**< MAC accumulator 2. */
    m68k_mac_acc3,                      /**< MAC accumulator 3. */
    m68k_mac_mask,                      /**< MAC mask register. */
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
};

enum M68kInstructionKind {
    m68k_unknown_instruction,
    m68k_add,
    m68k_adda,
    m68k_addi,
    m68k_addq,
    m68k_addx,
    m68k_and,
    m68k_andi,
    m68k_asl,
    m68k_asr,
    m68k_bcc,                           /**< branch carry clear (alias bhs) */
    m68k_bcs,                           /**< branch carry set (alias blo) */
    m68k_beq,                           /**< branch equal */
    m68k_bge,                           /**< branch greater or equal */
    m68k_bgt,                           /**< branch greater than */
    m68k_bhi,                           /**< branch high */
    m68k_ble,                           /**< branch less or equal */
    m68k_bls,                           /**< branch lower or same */
    m68k_blt,                           /**< branch less than */
    m68k_bmi,                           /**< branch minus */
    m68k_bne,                           /**< branch not equal */
    m68k_bpl,                           /**< branch plus */
    m68k_bvc,                           /**< branch overflow clear */
    m68k_bvs,                           /**< branch overflow set */
    m68k_bchg,
    m68k_bclr,
    m68k_bitrev,                        /**< Bit reverse register */
    m68k_bra,
    m68k_bset,
    m68k_bsr,
    m68k_btst,
    m68k_byterev,                       /**< Byte reverse register */
    m68k_clr,
    m68k_cmp,
    m68k_cmpa,
    m68k_cmpi,
    m68k_cpushl,
    m68k_divs,
    m68k_divu,
    m68k_eor,
    m68k_eori,
    m68k_ext,
    m68k_extb,
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
    m68k_ff1,
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
    m68k_halt,
    m68k_illegal,
    m68k_intouch,
    m68k_jmp,
    m68k_jsr,
    m68k_lea,
    m68k_link,
    m68k_lsl,
    m68k_lsr,
    m68k_mac,
    m68k_mov3q,
    m68k_movclr,
    m68k_move,
    m68k_movea,
    m68k_movec,
    m68k_movem,
    m68k_moveq,
    m68k_msac,
    m68k_muls,
    m68k_mulu,
    m68k_mvs,
    m68k_mvz,
    m68k_neg,
    m68k_negx,
    m68k_nop,
    m68k_not,
    m68k_or,
    m68k_ori,
    m68k_pea,
    m68k_pulse,
    m68k_rems,
    m68k_remu,
    m68k_rte,
    m68k_rts,
    m68k_sats,
    m68k_st,
    m68k_sf,
    m68k_shi,
    m68k_sls,
    m68k_scc,
    m68k_scs,
    m68k_sne,
    m68k_seq,
    m68k_svc,
    m68k_svs,
    m68k_spl,
    m68k_smi,
    m68k_sge,
    m68k_slt,
    m68k_sgt,
    m68k_sle,
    m68k_stop,
    m68k_sub,
    m68k_suba,
    m68k_subi,
    m68k_subq,
    m68k_subx,
    m68k_swap,
    m68k_tas,
    m68k_tpf,
    m68k_trap,
    m68k_tst,
    m68k_unlk,
    m68k_wddata,
    m68k_wdebug,

    // must be last
    m68k_last_instruction
};

#endif
