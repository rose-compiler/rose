#ifndef ROSE_BinaryAnalysis_InstructionEnumsM68k_H
#define ROSE_BinaryAnalysis_InstructionEnumsM68k_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

/* References:
 *   [1] "M68000 PM/AD REV.1 Programmers Reference Manual (Includes CPU32 Instructions)" Downloaded from the
 *       Freescale website on 2013-10-07.
 */

namespace Rose {
namespace BinaryAnalysis {

/** Members of the Motorola Coldfire family of m68k processors.  These descriptions come from page 1-1 of the manual. In
 *  particular, when the manual mentions a processor (like "MC68020") it refers to both that particular processor and the
 *  embedded versions thereof ("MC68EC020"). We'll use the same approach here, and if we need to refer to only a paricular
 *  processor excluding embedded version, we append "_only" to the name. */
enum M68kFamily {
    m68k_family         = 0xffffffff,                   /**< All M68k CPUs */
    // Generation one (internally 16/32-bit and produced with 8-, 16-, and 32-bit interfaces)
    m68k_generation_1   = 0x000000ff,                   /**< M68k first generation */
    m68k_68000_only     = 0x00000001,                   /**< MC68000 16-/32-bit microprocessor */
    m68k_68ec000        = 0x00000002,                   /**< MC68EC000 16-/32-bit embedded controller */
    m68k_68hc000        = 0x00000004,                   /**< MC68HC000 low-power CMOS version of MC68000 */
    m68k_68000          = 0x00000007,                   /**< MC68000 and embedded versions thereof */
    m68k_68008          = 0x00000008,                   /**< MC68008 16-bit microprocessor with 8-bit external data bus */
    m68k_68010          = 0x00000010,                   /**< MC68010 16-/32-bit virtual memory microprocessor */
    m68k_68012          = 0x00000020,                   /**< MC68012 84-pin PGA version of MC68010 supporting 2GB RAM */
    // Generation two (internally fully 32-bit)
    m68k_generation_2   = 0x0000ff00,                   /**< M68k second generation */
    m68k_68020_only     = 0x00000100,                   /**< MC68020 32-bit virtual memory microprocessor */
    m68k_68ec020        = 0x00000200,                   /**< MC68EC020 32-bit embedded controller */
    m68k_68020          = 0x00000300,                   /**< MC68020 and embedded versions thereof. */
    m68k_68030_only     = 0x00000400,                   /**< MC68030 second-generation 32-bit enhanced microprocessor */
    m68k_68ec030        = 0x00001000,                   /**< MC68EC030 32-bit embedded controller */
    m68k_68030          = 0x00002000,                   /**< MC68030 and embedded versions thereof */
    // Generation three (pipelined)
    m68k_generation_3   = 0x00ff0000,                   /**< M68k third generation */
    m68k_68040_only     = 0x00010000,                   /**< MC68040 third-generation 32-bit microprocessor */
    m68k_68ec040        = 0x00020000,                   /**< MC68EC040 32-bit embedded controller w/out FPU or MMU */
    m68k_68lc040        = 0x00040000,                   /**< MC68LC040 32-bit embedded controller w/out FPU */
    m68k_68040          = 0x00070000,                   /**< MC68040 and embedded versions thereof */
    // Freescale CPUs based on m68k
    m68k_freescale      = 0xff000000,                   /**< Freescale CPUs based on Motorola 683xx */
    m68k_freescale_cpu32= 0x01000000,                   /**< Freescale CPU32 (similar to MC68020 w/out bitfield insns */
    m68k_freescale_isaa = 0x02000000,                   /**< Freescale ISA_A, the original ColdFire ISA (subset of M68000) */
    m68k_freescale_isab = 0x04000000,                   /**< Freescale ISA_B, improved data movement instructions, etc. */
    m68k_freescale_isac = 0x08000000,                   /**< Freescale ISA_C, improved bit manipulation instructions */
    m68k_freescale_fpu  = 0x10000000,                   /**< Freescale FPU, original ColdFire floating point ISA. */
    m68k_freescale_mac  = 0x20000000,                   /**< Freescale MAC, original ColdFire multiply-accumulate ISA. */
    m68k_freescale_emac = 0x40000000,                   /**< Freescale EMAC, enhanced multiply-accumulate ISA. */
    m68k_freescale_emacb= 0x80000000                    /**< Freescale EMAC_B, dual accumulation instructions. */
};

/** M68k register classes. */
enum M68kRegisterClass {
    m68k_regclass_data,                                 /**< Data registers. */
    m68k_regclass_addr,                                 /**< Address registers. */
    m68k_regclass_fpr,                                  /**< Floating point registers. */
    m68k_regclass_spr,                                  /**< Special purpose registers. */
    m68k_regclass_mac,                                  /**< Multiply-accumulate registers (includes EMAC registers). */
    m68k_regclass_sup                                   /**< Supervisor registers. */
};

/** M68k special purpose registers. */
enum M68kSpecialPurposeRegister {
    m68k_spr_pc,                                        /**< Program counter. */
    m68k_spr_sr,                                        /**< Status register, including condition codes. */
    m68k_spr_fpcr,                                      /**< Floating-point control register. */
    m68k_spr_fpsr,                                      /**< Floating-point status register. */
    m68k_spr_fpiar                                      /**< Floating-point instruction address register. */
};

/** M68k MAC registers. */
enum M68kMacRegister {
    m68k_mac_macsr,                                     /**< MAC status register. */
    m68k_mac_acc0,                                      /**< MAC 32-bit accumulator #0. */
    m68k_mac_acc1,                                      /**< MAC 32-bit accumulator #1. */
    m68k_mac_acc2,                                      /**< MAC 32-bit accumulator #2. */
    m68k_mac_acc3,                                      /**< MAC 32-bit accumulator #3. */
    m68k_mac_ext01,                                     /**< Extensions for ACC0 and ACC1. */
    m68k_mac_ext23,                                     /**< Extensions for ACC2 and ACC3. */
    m68k_mac_ext0,                                      /**< Extension for ACC0. */
    m68k_mac_ext1,                                      /**< Extension for ACC1. */
    m68k_mac_ext2,                                      /**< Extension for ACC2. */
    m68k_mac_ext3,                                      /**< Extension for ACC3. */
    m68k_mac_mask                                       /**< MAC mask register. */
};

/** M68k EMAC registers. */
enum M68kEmacRegister {
    m68k_emac_macsr,                                    /**< EMAC status register. */
    m68k_emac_acc0,                                     /**< EMAC accumulator 0. */
    m68k_emac_acc1,                                     /**< EMAC accumulator 1. */
    m68k_emac_acc2,                                     /**< EMAC accumulator 2. */
    m68k_emac_acc3,                                     /**< EMAC accumulator 3. */
    m68k_emac_mask                                      /**< EMAC mask register. */
};

/** M68k supervisor registers. */
enum M68kSupervisorRegister {
    m68k_sup_vbr,                                       /**< Interrupt vector base address. */
    m68k_sup_ssp,                                       /**< Supervisor stack pointer. */
    m68k_sup_sfc,                                       /**< Alternate function. */
    m68k_sup_dfc,                                       /**< Code registers. */
    m68k_sup_cacr,                                      /**< Cache control register. */
    m68k_sup_asid,                                      /**< Address space ID register. */
    m68k_sup_acr0,                                      /**< Access control register 0 (data). */
    m68k_sup_acr1,                                      /**< Access control register 1 (data). */
    m68k_sup_acr2,                                      /**< Access control register 2 (instruction). */
    m68k_sup_acr3,                                      /**< Access control register 3 (instruction). */
    m68k_sup_mmubar,                                    /**< MMU base address register. */
    m68k_sup_rombar0,                                   /**< ROM base address register 0. */
    m68k_sup_rombar1,                                   /**< ROM base address register 1. */
    m68k_sup_rambar0,                                   /**< RAM base address register 0. */
    m68k_sup_rambar1,                                   /**< RAM base address register 1. */
    m68k_sup_mbar,                                      /**< Module base address register. */
    m68k_sup_mpcr,                                      /**< Multiprocessor control register. */
    m68k_sup_edrambar,                                  /**< Embedded DRAM base address register. */
    m68k_sup_secmbar,                                   /**< Secondary module base address register. */
    m68k_sup_0_pcr1,                                    /**< RAM 0 permutation control register 1. */
    m68k_sup_0_pcr2,                                    /**< RAM 0 permutation control register 2. */
    m68k_sup_0_pcr3,                                    /**< RAM 0 permutation control register 3. */
    m68k_sup_1_pcr1,                                    /**< RAM 1 permutation control register 1. */
    m68k_sup_1_pcr2,                                    /**< RAM 1 permutation control register 2. */
    m68k_sup_1_pcr3                                     /**< RAM 1 permutation control register 3. */
};

/** M68k effective addressing modes.
 *
 * @code
 *                                                 Mode   Reg.
 * Addressing Modes                Syntax          Field  Field  Data Mem Ctl Alter 234  Symbol
 * ------------------------------- --------------- -----  -----  ---- --- --- ----- ---  -----------------
 * Register direct                                                                       m68k_eam_rd
 *     Data                        Dn               000  reg#    X    -   -   X     -    m68k_eam_drd
 *     Address                     An               001  reg#    -    -   -   X     -    m68k_eam_ard
 *
 * Register indirect                                                                     m68k_eam_ri
 *     Address                     (An)             010  reg#    X    X   X   X     -    m68k_eam_ari
 *     Address with postincrement  (An)+            011  reg#    X    X   -   X     -    m68k_eam_inc
 *     Address with predecrement   -(An)            100  reg#    X    X   -   X     -    m68k_eam_dec
 *     Address with displacement   (d16,An)         101  reg#    X    X   X   X     -    m68k_eam_dsp
 *
 * Address register indirect with index                                                  m68k_eam_idx
 *     8-bit displacement          (d8,An,Xn)       110  reg#    X    X   X   X     -    m68k_eam_idx8
 *     Base Displacement           (bd,An,Xn)       110  reg#    X    X   X   X     X    m68k_eam_idxbd
 *
 * Memory indirect                                                                       m68k_eam_mi
 *     Postindexed                 ([bd,An],Xn,od)  110  reg#    X    X   X   X     X    m68k_eam_mpost
 *     Preindexed                  ([bd,An,Xn],od)  110  reg#    X    X   X   X     X    m68k_eam_mpre
 *
 * Program counter indirect                                                              m68k_eam_pci
 *     With displacement           (d16,PC)         111  010     X    X   X   -     -    m68k_eam_pcdsp
 *
 * Program counter indirect with index                                                   m68k_eam_pcidx
 *     8-Bit Displacement          (d8,PC,Xn)       111  011     X    X   X   -     -    m68k_eam_pcidx8
 *     Base Displacement           (bd,PC,Xn)       111  011     X    X   X   -     X    m68k_eam_pcidxbd
 *
 * Program counter memory indirect                                                       m68k_eam_pcmi
 *     Postindexed                 ([bd,PC],Xn,od)  111  011     X    X   X   X(+)  X    m68k_eam_pcmpost
 *     Preindexed                  ([bd,PC,Xn],od)  111  011     X    X   X   X(+)  X    m68k_eam_pcmpre
 *
 * Absolute data addressing                                                              m68k_eam_abs
 *     Short                       (xxx).W          111  000     X    X   X   X(+)  -    m68k_eam_absw
 *     Long                        (xxx).L          111  001(*)  X    X   X   X(+)  -    m68k_eam_absl
 *
 * Immediate                       #<xxx>           111  100     X    X   -   -     -    m68k_eam_imm
 *
 * Unused
 *     NA                          NA               111  101     -    -   -   -     -
 *     NA                          NA               111  110     -    -   -   -     -
 *     NA                          NA               111  111     -    -   -   -     -
 * @endcode
 *
 * Note *: the "reg field" for absolute data addressing long is indicated as "000" in the documentation but I believe it should
 * be "001". [Robb P. Matzke 2013-10-07]
 *
 * Note +: the absolute data addressing modes are marked as non-alterable in this table in the m68k documentation, but the
 * documentation for instructions that say an operand "uses the alterable addressing modes shown in the following table" shows
 * the absolute addressing modes as being valid for the instruction. Therefore, I'm including the absolute addressing modes in
 * the set of alterable addressing modes. [Robb P. Matzke 2013-10-07]   After implementing all the integer instructions, it
 * looks like this table in the reference manual has an error: the table excludes the two absolute data addressing modes (as
 * just mentioned), but includes the two program counter memory indirect modes (which are missing from the tables for most
 * instructions that say they use alterable addressing modes). So it looks like someone at Motorola placed two "X"s in the
 * wrong rows. I may change my table in the future. FIXME[Robb P. Matzke 2014-02-24]
 */
enum M68kEffectiveAddressMode {
    m68k_eam_unknown = 0,

    // single bits
    m68k_eam_drd     = 0x00000001,      /**< Data register direct: Dn */
    m68k_eam_ard     = 0x00000002,      /**< Address register direct: An */
    m68k_eam_ari     = 0x00000004,      /**< Address register indirect: (An) */
    m68k_eam_inc     = 0x00000008,      /**< Address register indirect with post increment: (An)+ */
    m68k_eam_dec     = 0x00000010,      /**< Address register indirect with pre decrement: -(An) */
    m68k_eam_dsp     = 0x00000020,      /**< Address register indirect with displacement: (d_16,An) */
    m68k_eam_idx8    = 0x00000040,      /**< Address register indirect with scaled index and 8-bit displacement */
    m68k_eam_idxbd   = 0x00000080,      /**< Address register indirect with scaled index and base displacement */
    m68k_eam_mpost   = 0x00000100,      /**< Memory indirect post indexed */
    m68k_eam_mpre    = 0x00000200,      /**< Memory indirect pre indexed */
    m68k_eam_pcdsp   = 0x00000400,      /**< Program counter indirect with displacement: (d_16,PC) */
    m68k_eam_pcidx8  = 0x00000800,      /**< Program counter indirect with scaled index and 8-bit displacement */
    m68k_eam_pcidxbd = 0x00001000,      /**< Program counter indirect with scaled index and base displacement */
    m68k_eam_pcmpost = 0x00002000,      /**< Program counter memory indirect post indexed */
    m68k_eam_pcmpre  = 0x00004000,      /**< Program counter memory indirect pre indexed */
    m68k_eam_absw    = 0x00008000,      /**< Absolute data addressing short: (xxx).W */
    m68k_eam_absl    = 0x00010000,      /**< Absolute data addressing long: (xxx).L */
    m68k_eam_imm     = 0x00020000,      /**< Immediate data */

    // masks for groups of rows from the table above.
    m68k_eam_all     = 0x0003ffff,      /**< All addressing modes */
    m68k_eam_rd      = 0x00000003,      /**< Register direct addressing modes */
    m68k_eam_ri      = 0x0000003c,      /**< Register indirect addressing modes */
    m68k_eam_idx     = 0x000000c0,      /**< Address register indirect with scaled index */
    m68k_eam_mi      = 0x00000300,      /**< Memory indirect addressing modes */
    m68k_eam_pci     = 0x00000400,      /**< Program counter indirect */                                // NO_STRINGIFY
    m68k_eam_pcidx   = 0x00001800,      /**< Program counter indirect with index */
    m68k_eam_pcmi    = 0x00006000,      /**< Program counter memory indirect */
    m68k_eam_abs     = 0x00018000,      /**< Absolute data addressing */

    // masks for the data, mem, ctl, alter, and 234 columns of the table above.
    m68k_eam_data    = 0x0003fffd,      /**< Data addressing modes. All modes except address register direct. */
    m68k_eam_memory  = 0x0003fffc,      /**< Memory addressing modes. All modes except register direct. */
    m68k_eam_control = 0x0001ffe4,      /**< Control addressing modes. Memory modes except increment, decrement, and
                                         *   immediate. */
    m68k_eam_alter   = 0x0001e3ff,      /**< Alterable addressing modes. All modes except immediate, program counter
                                         *   indirect with index, and program counter indirect. Note that the m68k
                                         *   documentation excludes absolute addressing modes from the definition of alterable
                                         *   modes, but then includes the absolute addressing modes in all the instructions
                                         *   that say they use alterable addressing modes. */
    m68k_eam_234     = 0x00007380,      /**< Addressing modes specific to m680{20,30,40}. */

    // additional useful masks
    m68k_eam_direct  = 0x00000003,      /**< All register direct addressing modes. */                   // NO_STRINGIFY
    m68k_eam_pc      = 0x00007c00       /**< All PC address modes. */
};

/** M68k data formats for floating-point operations.
 *
 *  The integer values are important: they are the bit patterns in floating point instruction encodings. */
enum M68kDataFormat {
    m68k_fmt_i32 = 0,                                   /**< 32-bit integer. */
    m68k_fmt_f32 = 1,                                   /**< 32-bit floating point, "single real". */
    m68k_fmt_f96 = 2,                                   /**< 96-bit floating point, "extended real". */
    m68k_fmt_p96 = 3,                                   /**< Three 32-bit words of binary coded decimal. */
    m68k_fmt_i16 = 4,                                   /**< 16-bit integer. */
    m68k_fmt_f64 = 5,                                   /**< 64-bit floating point, "double real". */
    m68k_fmt_i8  = 6,                                   /**< 8-bit integer. */
    m68k_fmt_unknown = 255                              /**< Invalid format. */
};

/** M68k instruction types. */
enum M68kInstructionKind {
    m68k_unknown_instruction,
    m68k_abcd,                                          /**< Add decimal with extended */
    m68k_add,                                           /**< Add */
    m68k_adda,                                          /**< Add address */
    m68k_addi,                                          /**< Add immediate */
    m68k_addq,                                          /**< Add quick */
    m68k_addx,                                          /**< Add extended */
    m68k_and,                                           /**< AND logical */
    m68k_andi,                                          /**< AND immediate */
    m68k_asl,                                           /**< Arithmetic shift left */
    m68k_asr,                                           /**< Arithmetic shift right */
    m68k_bcc,                                           /**< Branch carry clear (alias bhs) */
    m68k_bcs,                                           /**< Branch carry set (alias blo) */
    m68k_beq,                                           /**< Branch equal */
    m68k_bge,                                           /**< Branch greater or equal */
    m68k_bgt,                                           /**< Branch greater than */
    m68k_bhi,                                           /**< Branch high */
    m68k_ble,                                           /**< Branch less or equal */
    m68k_bls,                                           /**< Branch lower or same */
    m68k_blt,                                           /**< Branch less than */
    m68k_bmi,                                           /**< Branch minus */
    m68k_bne,                                           /**< Branch not equal */
    m68k_bpl,                                           /**< Branch plus */
    m68k_bvc,                                           /**< Branch overflow clear */
    m68k_bvs,                                           /**< Branch overflow set */
    m68k_bchg,                                          /**< Test a bit and change */
    m68k_bclr,                                          /**< Test a bit and clear */
    m68k_bfchg,                                         /**< Test bit field and change */
    m68k_bfclr,                                         /**< Test bit field and clear */
    m68k_bfexts,                                        /**< Extract bit field signed */
    m68k_bfextu,                                        /**< Extract bit field unsigned */
//    m68k_bfffo,                         /**< Find first one in bit field */
    m68k_bfins,                                         /**< Insert bit field */
    m68k_bfset,                                         /**< Test bit field and set */
    m68k_bftst,                                         /**< Test bit field */
//    m68k_bitrev,                        /**< Bit reverse register */
    m68k_bkpt,                                          /**< Breakpoint */
    m68k_bra,                                           /**< Branch always */
    m68k_bset,                                          /**< Test a bit and set */
    m68k_bsr,                                           /**< Branch to subroutine */
    m68k_btst,                                          /**< Test a bit */
//    m68k_byterev,                       /**< Byte reverse register */
    m68k_callm,                                         /**< Call module */
    m68k_cas,                                           /**< Compare and swap with operand */
    m68k_cas2,                                          /**< Compare and swap with operands */
    m68k_chk,                                           /**< Check register against bounds */
    m68k_chk2,                                          /**< Check register against bounds */
    m68k_clr,                                           /**< Clear an operand */
    m68k_cmp,                                           /**< Compare */
    m68k_cmp2,                                          /**< Compare register against bounds */
    m68k_cmpa,                                          /**< Compare address */
    m68k_cmpi,                                          /**< Compare immediate */
    m68k_cmpm,                                          /**< Compare memory */
    m68k_cpusha,                                        /**< Push and invalidate all */
    m68k_cpushl,                                        /**< Push and invalidate cache lines */
    m68k_cpushp,                                        /**< Push and invalidate cache pages */
    m68k_dbt,                                           /**< Decrement and branch if true */
    m68k_dbf,                                           /**< Decrement and branch if false */
    m68k_dbhi,                                          /**< Decrement and branch if high */
    m68k_dbls,                                          /**< Decrement and branch if lower or same */
    m68k_dbcc,                                          /**< Decrement and branch if cary clear */
    m68k_dbcs,                                          /**< Decrement and branch if carry set */
    m68k_dbne,                                          /**< Decrement and branch if not equal */
    m68k_dbeq,                                          /**< Decrement and branch if equal */
    m68k_dbvc,                                          /**< Decrement and branch if overflow clear */
    m68k_dbvs,                                          /**< Decrement and branch if overflow set */
    m68k_dbpl,                                          /**< Decrement and branch if plus */
    m68k_dbmi,                                          /**< Decrement and branch if minus */
    m68k_dbge,                                          /**< Decrement and branch if greater or equal */
    m68k_dblt,                                          /**< Decrement and branch if less than */
    m68k_dbgt,                                          /**< Decrement and branch if greater than */
    m68k_dble,                                          /**< Decrement and branch if less than or equal */
    m68k_divs,                                          /**< Signed divide with optional remainder */
    m68k_divsl,                                         /**< Signed divide 32-bit quotient with remainder */
    m68k_divu,                                          /**< Unsigned divide with optional remainder */
    m68k_divul,                                         /**< Unsigned divide 32-bit quotient with remainder */
    m68k_eor,                                           /**< Exclusive-OR logical */
    m68k_eori,                                          /**< Exclusive-OR immediate */
    m68k_exg,                                           /**< Exchange registers */
    m68k_ext,                                           /**< Sign extend */
    m68k_extb,                                          /**< Sign extend byte to longword */
    m68k_fabs,                                          /**< Floating-point absolute value with FPCR rounding */
    m68k_fadd,                                          /**< Floating-point add with FPCR rounding */
    m68k_fbeq,                                          /**< Floating-point branch if equal */
    m68k_fbne,                                          /**< Floating-point branch if not equal */
    m68k_fbgt,                                          /**< Floating-point branch if greater than */
    m68k_fbngt,                                         /**< Floating-point branch if not greater than */
    m68k_fbge,                                          /**< Floating-point branch if greater than or equal */
    m68k_fbnge,                                         /**< Floating-point branch if not greater than or equal */
    m68k_fblt,                                          /**< Floating-point branch if less than */
    m68k_fbnlt,                                         /**< Floating-point branch if not less than */
    m68k_fble,                                          /**< Floating-point branch if less than or equal */
    m68k_fbnle,                                         /**< Floating-point branch if not less than or equal */
    m68k_fbgl,                                          /**< Floating-point branch if greater or less than */
    m68k_fbngl,                                         /**< Floating-point branch if not greater or less than */
    m68k_fbgle,                                         /**< Floating-point branch if greater, less, or equal */
    m68k_fbngle,                                        /**< Floating-point branch if not greater, less, or equal */
    m68k_fbogt,                                         /**< Floating-point branch if ordered greater than */
    m68k_fbule,                                         /**< Floating-point branch if unordered or less or equal */
    m68k_fboge,                                         /**< Floating-point branch if ordered greater than or equal */
    m68k_fbult,                                         /**< Floating-point branch if unordered less than */
    m68k_fbolt,                                         /**< Floating-point branch if ordered less than */
    m68k_fbuge,                                         /**< Floating-point branch if unordered or greater than or equal */
    m68k_fbole,                                         /**< Floating-point branch if ordered less than or equal */
    m68k_fbugt,                                         /**< Floating-point branch if unordered or greater than */
    m68k_fbogl,                                         /**< Floating-point branch if ordered greater or less than */
    m68k_fbueq,                                         /**< Floating-point branch if unordered or equal */
    m68k_fbor,                                          /**< Floating-point branch if ordered */
    m68k_fbun,                                          /**< Floating-point branch if unordered */
    m68k_fbf,                                           /**< Floating-point branch if false */
    m68k_fbt,                                           /**< Floating-point branch if true */
    m68k_fbsf,                                          /**< Floating-point branch if signaling false */
    m68k_fbst,                                          /**< Floating-point branch if signaling true */
    m68k_fbseq,                                         /**< Floating-point branch if signaling equal */
    m68k_fbsne,                                         /**< Floating-point branch if signaling not equal */
    m68k_fcmp,                                          /**< Floating-point compare */
    m68k_fdabs,                                         /**< Floating-point absolute value with double-precision rounding */
    m68k_fdadd,                                         /**< Floating-point add with double-precision rounding */
    m68k_fddiv,                                         /**< Floating-point divide with double-precision rounding */
    m68k_fdiv,                                          /**< Floating-point divide with FPCR rounding */
    m68k_fdmove,                                        /**< Move floating-point data with double-precision rounding */
    m68k_fdmul,                                         /**< Floating-point multiple with double-precision rounding */
    m68k_fdneg,                                         /**< Floating-point negation with double-precision rounding */
    m68k_fdsqrt,                                        /**< Floating-point square root with double-precision rounding */
    m68k_fdsub,                                         /**< Floating-point subtract with double-precision rounding */
//    m68k_ff1,                           /**< Find first one in register */
    m68k_fint,                                          /**< Floating-point integer part */
    m68k_fintrz,                                        /**< Floating-point integer part rounded-to-zero */
    m68k_fmove,                                         /**< Move floating-point data with FPCR rounding */
    m68k_fmovem,                                        /**< Move multiple floating-point data registers */
    m68k_fmul,                                          /**< Floating-point multiply with FPCR rounding */
    m68k_fneg,                                          /**< Floating-pont negate with FPCR rounding */
    m68k_fnop,                                          /**< Floating-point no operation */
//    m68k_frestore,
    m68k_fsabs,                                         /**< Floating-point absolute value with single-precision rounding */
    m68k_fsadd,                                         /**< Floating-point add with single-precision rounding */
//    m68k_fsave,
    m68k_fsdiv,                                         /**< Floating-point divide with single-precision rounding */
    m68k_fsmove,                                        /**< Move floating-point data with single-precision rounding */
    m68k_fsmul,                                         /**< Floating-point multiply with single-precision rounding */
    m68k_fsneg,                                         /**< Floating-point negation with single-precision rounding */
    m68k_fsqrt,                                         /**< Floating-point square root with FPCR rounding */
    m68k_fssqrt,                                        /**< Floating-point square root with single-precision rounding */
    m68k_fssub,                                         /**< Floating-point subtract with single-precision rounding */
    m68k_fsub,                                          /**< Floating-point subtract with FPCR rounding */
    m68k_ftst,                                          /**< Floating point test */
//    m68k_halt,                          /**< Halt the CPU */
    m68k_illegal,                                       /**< Take illegal instruction trap */
//    m68k_intouch,
    m68k_jmp,                                           /**< Jump */
    m68k_jsr,                                           /**< Jump to subroutine */
    m68k_lea,                                           /**< Load effective address */
    m68k_link,                                          /**< Link and allocate */
    m68k_lsl,                                           /**< Logical shift left */
    m68k_lsr,                                           /**< Logical shift right */
    m68k_mac,                                           /**< Multiply accumulate */
    m68k_mov3q,                                         /**< Move 3-bit data quick */
    m68k_movclr,                                        /**< Move from MAC ACC register and clear */
    m68k_move,                                          /**< Move from source to destination (data, CCR, ACC, MACSR, MASK) */
    m68k_move_acc,                                      /**< Move MAC ACC register */
    m68k_move_accext,                                   /**< Move MAC ACCext register */
    m68k_move_ccr,                                      /**< Move condition code register */
    m68k_move_macsr,                                    /**< Move MAC status register */
    m68k_move_mask,                                     /**< Move MAC MASK register */
    m68k_move_sr,                                       /**< Move status register */
    m68k_move16,                                        /**< Move 16-byte block */
    m68k_movea,                                         /**< Move address from source to destination */
    m68k_movec,                                         /**< Move control register */
    m68k_movem,                                         /**< Move multiple registers */
    m68k_movep,                                         /**< Move peripheral data */
    m68k_moveq,                                         /**< Move quick */
    m68k_msac,                                          /**< Multiply subtract */
    m68k_muls,                                          /**< Signed multiply */
    m68k_mulu,                                          /**< Unsigned multiply */
    m68k_mvs,                                           /**< Move with sign extend */
    m68k_mvz,                                           /**< Move with zero fill */
    m68k_nbcd,                                          /**< Negate decimal with extend */
    m68k_neg,                                           /**< Negate */
    m68k_negx,                                          /**< Negate with extend */
    m68k_nop,                                           /**< No operation */
    m68k_not,                                           /**< Logical complement */
    m68k_or,                                            /**< Inclusive-OR logical */
    m68k_ori,                                           /**< Inclusive-OR immediate */
    m68k_pack,                                          /**< Pack */
    m68k_pea,                                           /**< Push effective address */
//    m68k_pulse,                         /**< Generate unique processor status */
//    m68k_rems,                          /**< Signed divide remainder -- see divs instead */
//    m68k_remu,                          /**< Unsigned divide remainder -- see divu instead */
    m68k_rol,                                           /**< Rotate left without extend */
    m68k_ror,                                           /**< Rotate right without extend */
    m68k_roxl,                                          /**< Rotate left with extend */
    m68k_roxr,                                          /**< Rotate right with extend */
    m68k_rtd,                                           /**< Return and deallocate */
    m68k_rtm,                                           /**< Return from module */
//    m68k_rte,                           /**< Return from exception */
    m68k_rtr,                                           /**< Return and restore condition codes */
    m68k_rts,                                           /**< Return from subroutine */
//    m68k_sats,                          /**< Signed saturate */
    m68k_sbcd,                                          /**< Subtract decimal with extend */
    m68k_st,                                            /**< Set if true */
    m68k_sf,                                            /**< Set if false */
    m68k_shi,                                           /**< Set if high */
    m68k_sls,                                           /**< Set if lower or same */
    m68k_scc,                                           /**< Set if carry clear (HS) */
    m68k_scs,                                           /**< Set if carry set (LO) */
    m68k_sne,                                           /**< Set if not equal */
    m68k_seq,                                           /**< Set if equal */
    m68k_svc,                                           /**< Set if overflow clear */
    m68k_svs,                                           /**< Set if overflow set */
    m68k_spl,                                           /**< Set if plus */
    m68k_smi,                                           /**< Set if minus */
    m68k_sge,                                           /**< Set if greater or equal */
    m68k_slt,                                           /**< Set if less than */
    m68k_sgt,                                           /**< Set if greater than */
    m68k_sle,                                           /**< Set if less or equal */
//    m68k_stop,
    m68k_sub,                                           /**< Subtract */
    m68k_suba,                                          /**< Subtract address */
    m68k_subi,                                          /**< Subtract immediate */
    m68k_subq,                                          /**< Subtract quick */
    m68k_subx,                                          /**< Subtract extended */
    m68k_swap,                                          /**< Swap register halves */
    m68k_tas,                                           /**< Test and set an operand */
//    m68k_tpf,                           /**< Trap false (no operation) */
    m68k_trap,                                          /**< Trap */
    m68k_trapt,                                         /**< Trap if true */
    m68k_trapf,                                         /**< Trap if false */
    m68k_traphi,                                        /**< Trap if high */
    m68k_trapls,                                        /**< Trap if lower or same */
    m68k_trapcc,                                        /**< Trap if carry clear (HS) */
    m68k_trapcs,                                        /**< Trap if carry set (LO) */
    m68k_trapne,                                        /**< Trap if not equal */
    m68k_trapeq,                                        /**< Trap if equal */
    m68k_trapvc,                                        /**< Trap if overflow clear */
    m68k_trapvs,                                        /**< Trap if overflow set */
    m68k_trappl,                                        /**< Trap if plus */
    m68k_trapmi,                                        /**< Trap if minus */
    m68k_trapge,                                        /**< Trap if greater or equal */
    m68k_traplt,                                        /**< Trap if less than */
    m68k_trapgt,                                        /**< Trap if greater than */
    m68k_traple,                                        /**< Trap if less or equal */
    m68k_trapv,                                         /**< Trap on overflow */
    m68k_tst,                                           /**< Test an operand */
    m68k_unlk,                                          /**< Unlink */
    m68k_unpk,                                          /**< Unpack binary coded decimal */
//    m68k_wddata,                        /**< Write to debug data */
//    m68k_wdebug,

    // must be last
    m68k_last_instruction
};

} // namespace
} // namespace

#endif
#endif
