#ifndef ROSE_BinaryAnalysis_InstructionEnumsCil_H
#define ROSE_BinaryAnalysis_InstructionEnumsCil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

/* References:
 *   [1] "ECMA Standard ECMA-335 Common Language Infrastructure (CLI)" Downloaded 
     from https://www.ecma-international.org/publications-and-standards/standards/ecma-335/
 */

namespace Rose {
namespace BinaryAnalysis {

/** Members of the Motorola Coldfire family of m68k processors.  These descriptions come from page 1-1 of the manual. In
 *  particular, when the manual mentions a processor (like "MC68020") it refers to both that particular processor and the
 *  embedded versions thereof ("MC68EC020"). We'll use the same approach here, and if we need to refer to only a paricular
 *  processor excluding embedded version, we append "_only" to the name. */
enum CilFamily {
    Cil_family         = 0xffffffff,                   /**< All CIL Instruction sets */
};

/** CIL register classes (unclear if these exist) */
enum CilRegisterClass {
    Cil_regclass_data,                                 /**< Data registers. */
    Cil_regclass_addr,                                 /**< Address registers. */
    Cil_regclass_fpr,                                  /**< Floating point registers. */
    Cil_regclass_spr,                                  /**< Special purpose registers. */
    Cil_regclass_mac,                                  /**< Multiply-accumulate registers (includes EMAC registers). */
    Cil_regclass_sup                                   /**< Supervisor registers. */
};

/** CIL special purpose registers.  (unclear if these exist) */
enum CilSpecialPurposeRegister {
    Cil_spr_pc,                                        /**< Program counter. */
    Cil_spr_sr,                                        /**< Status register, including condition codes. */
    Cil_spr_fpcr,                                      /**< Floating-point control register. */
    Cil_spr_fpsr,                                      /**< Floating-point status register. */
    Cil_spr_fpiar                                      /**< Floating-point instruction address register. */
};

/** CIL MAC registers.  (unclear if these exist) */
enum CilMacRegister {
    Cil_mac_macsr,                                     /**< MAC status register. */
    Cil_mac_acc0,                                      /**< MAC 32-bit accumulator #0. */
    Cil_mac_acc1,                                      /**< MAC 32-bit accumulator #1. */
    Cil_mac_acc2,                                      /**< MAC 32-bit accumulator #2. */
    Cil_mac_acc3,                                      /**< MAC 32-bit accumulator #3. */
    Cil_mac_ext01,                                     /**< Extensions for ACC0 and ACC1. */
    Cil_mac_ext23,                                     /**< Extensions for ACC2 and ACC3. */
    Cil_mac_ext0,                                      /**< Extension for ACC0. */
    Cil_mac_ext1,                                      /**< Extension for ACC1. */
    Cil_mac_ext2,                                      /**< Extension for ACC2. */
    Cil_mac_ext3,                                      /**< Extension for ACC3. */
    Cil_mac_mask                                       /**< MAC mask register. */
};

/** CIL EMAC registers.  (unclear if these exist) */
enum CilEmacRegister {
    Cil_emac_macsr,                                    /**< EMAC status register. */
    Cil_emac_acc0,                                     /**< EMAC accumulator 0. */
    Cil_emac_acc1,                                     /**< EMAC accumulator 1. */
    Cil_emac_acc2,                                     /**< EMAC accumulator 2. */
    Cil_emac_acc3,                                     /**< EMAC accumulator 3. */
    Cil_emac_mask                                      /**< EMAC mask register. */
};

/** CIL supervisor registers.  (unclear if these exist) */
enum CilSupervisorRegister {
    Cil_sup_vbr,                                       /**< Interrupt vector base address. */
    Cil_sup_ssp,                                       /**< Supervisor stack pointer. */
    Cil_sup_sfc,                                       /**< Alternate function. */
    Cil_sup_dfc,                                       /**< Code registers. */
    Cil_sup_cacr,                                      /**< Cache control register. */
    Cil_sup_asid,                                      /**< Address space ID register. */
    Cil_sup_acr0,                                      /**< Access control register 0 (data). */
    Cil_sup_acr1,                                      /**< Access control register 1 (data). */
    Cil_sup_acr2,                                      /**< Access control register 2 (instruction). */
    Cil_sup_acr3,                                      /**< Access control register 3 (instruction). */
    Cil_sup_mmubar,                                    /**< MMU base address register. */
    Cil_sup_rombar0,                                   /**< ROM base address register 0. */
    Cil_sup_rombar1,                                   /**< ROM base address register 1. */
    Cil_sup_rambar0,                                   /**< RAM base address register 0. */
    Cil_sup_rambar1,                                   /**< RAM base address register 1. */
    Cil_sup_mbar,                                      /**< Module base address register. */
    Cil_sup_mpcr,                                      /**< Multiprocessor control register. */
    Cil_sup_edrambar,                                  /**< Embedded DRAM base address register. */
    Cil_sup_secmbar,                                   /**< Secondary module base address register. */
    Cil_sup_0_pcr1,                                    /**< RAM 0 permutation control register 1. */
    Cil_sup_0_pcr2,                                    /**< RAM 0 permutation control register 2. */
    Cil_sup_0_pcr3,                                    /**< RAM 0 permutation control register 3. */
    Cil_sup_1_pcr1,                                    /**< RAM 1 permutation control register 1. */
    Cil_sup_1_pcr2,                                    /**< RAM 1 permutation control register 2. */
    Cil_sup_1_pcr3                                     /**< RAM 1 permutation control register 3. */
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
enum CilEffectiveAddressMode {
    Cil_eam_unknown = 0,

    // single bits
    Cil_eam_drd     = 0x00000001,      /**< Data register direct: Dn */
    Cil_eam_ard     = 0x00000002,      /**< Address register direct: An */
    Cil_eam_ari     = 0x00000004,      /**< Address register indirect: (An) */
    Cil_eam_inc     = 0x00000008,      /**< Address register indirect with post increment: (An)+ */
    Cil_eam_dec     = 0x00000010,      /**< Address register indirect with pre decrement: -(An) */
    Cil_eam_dsp     = 0x00000020,      /**< Address register indirect with displacement: (d_16,An) */
    Cil_eam_idx8    = 0x00000040,      /**< Address register indirect with scaled index and 8-bit displacement */
    Cil_eam_idxbd   = 0x00000080,      /**< Address register indirect with scaled index and base displacement */
    Cil_eam_mpost   = 0x00000100,      /**< Memory indirect post indexed */
    Cil_eam_mpre    = 0x00000200,      /**< Memory indirect pre indexed */
    Cil_eam_pcdsp   = 0x00000400,      /**< Program counter indirect with displacement: (d_16,PC) */
    Cil_eam_pcidx8  = 0x00000800,      /**< Program counter indirect with scaled index and 8-bit displacement */
    Cil_eam_pcidxbd = 0x00001000,      /**< Program counter indirect with scaled index and base displacement */
    Cil_eam_pcmpost = 0x00002000,      /**< Program counter memory indirect post indexed */
    Cil_eam_pcmpre  = 0x00004000,      /**< Program counter memory indirect pre indexed */
    Cil_eam_absw    = 0x00008000,      /**< Absolute data addressing short: (xxx).W */
    Cil_eam_absl    = 0x00010000,      /**< Absolute data addressing long: (xxx).L */
    Cil_eam_imm     = 0x00020000,      /**< Immediate data */

    // masks for groups of rows from the table above.
    Cil_eam_all     = 0x0003ffff,      /**< All addressing modes */
    Cil_eam_rd      = 0x00000003,      /**< Register direct addressing modes */
    Cil_eam_ri      = 0x0000003c,      /**< Register indirect addressing modes */
    Cil_eam_idx     = 0x000000c0,      /**< Address register indirect with scaled index */
    Cil_eam_mi      = 0x00000300,      /**< Memory indirect addressing modes */
    Cil_eam_pci     = 0x00000400,      /**< Program counter indirect */                                // NO_STRINGIFY
    Cil_eam_pcidx   = 0x00001800,      /**< Program counter indirect with index */
    Cil_eam_pcmi    = 0x00006000,      /**< Program counter memory indirect */
    Cil_eam_abs     = 0x00018000,      /**< Absolute data addressing */

    // masks for the data, mem, ctl, alter, and 234 columns of the table above.
    Cil_eam_data    = 0x0003fffd,      /**< Data addressing modes. All modes except address register direct. */
    Cil_eam_memory  = 0x0003fffc,      /**< Memory addressing modes. All modes except register direct. */
    Cil_eam_control = 0x0001ffe4,      /**< Control addressing modes. Memory modes except increment, decrement, and
                                         *   immediate. */
    Cil_eam_alter   = 0x0001e3ff,      /**< Alterable addressing modes. All modes except immediate, program counter
                                         *   indirect with index, and program counter indirect. Note that the m68k
                                         *   documentation excludes absolute addressing modes from the definition of alterable
                                         *   modes, but then includes the absolute addressing modes in all the instructions
                                         *   that say they use alterable addressing modes. */
    Cil_eam_234     = 0x00007380,      /**< Addressing modes specific to m680{20,30,40}. */

    // additional useful masks
    Cil_eam_direct  = 0x00000003,      /**< All register direct addressing modes. */                   // NO_STRINGIFY
    Cil_eam_pc      = 0x00007c00       /**< All PC address modes. */
};

/** M68k data formats for floating-point operations.
 *
 *  The integer values are important: they are the bit patterns in floating point instruction encodings. */
enum CilDataFormat {
    Cil_fmt_i32 = 0,                                   /**< 32-bit integer. */
    Cil_fmt_f32 = 1,                                   /**< 32-bit floating point, "single real". */
    Cil_fmt_f96 = 2,                                   /**< 96-bit floating point, "extended real". */
    Cil_fmt_p96 = 3,                                   /**< Three 32-bit words of binary coded decimal. */
    Cil_fmt_i16 = 4,                                   /**< 16-bit integer. */
    Cil_fmt_f64 = 5,                                   /**< 64-bit floating point, "double real". */
    Cil_fmt_i8  = 6,                                   /**< 8-bit integer. */
    Cil_fmt_unknown = 255                              /**< Invalid format. */
};

/** CIL instruction types */
enum CilInstructionKind {
  Cil_unknown_instruction,
  Cil_nop,           // name="nop",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x00",flow="next",type="Primitive"
  Cil_break,         // name="break",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x01",flow="break",type="Primitive"
  Cil_ldarg_0,       // name="ldarg.0",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x02",flow="next",type="Macro",constant="0"
  Cil_ldarg_1,       // name="ldarg.1",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x03",flow="next",type="Macro",constant="1"
  Cil_ldarg_2,       // name="ldarg.2",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x04",flow="next",type="Macro",constant="2"
  Cil_ldarg_3,       // name="ldarg.3",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x05",flow="next",type="Macro",constant="3"
  Cil_ldloc_0,       // name="ldloc.0",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x06",flow="next",type="Macro",constant="0"
  Cil_ldloc_1,       // name="ldloc.1",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x07",flow="next",type="Macro",constant="1"
  Cil_ldloc_2,       // name="ldloc.2",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x08",flow="next",type="Macro",constant="2"
  Cil_ldloc_3,       // name="ldloc.3",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x09",flow="next",type="Macro",constant="3"
  Cil_stloc_0,       // name="stloc.0",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0A",flow="next",type="Macro",constant="0"
  Cil_stloc_1,       // name="stloc.1",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0B",flow="next",type="Macro",constant="1"
  Cil_stloc_2,       // name="stloc.2",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0C",flow="next",type="Macro",constant="2"
  Cil_stloc_3,       // name="stloc.3",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0D",flow="next",type="Macro",constant="3"
  Cil_ldarg_s,       // name="ldarg.s",input="Pop0",output="Push1",args="ShortInlineVar",o1="0xFF",o2="0x0E",flow="next",type="Macro"
  Cil_ldarga_s,      // name="ldarga.s",input="Pop0",output="PushI",args="ShortInlineVar",o1="0xFF",o2="0x0F",flow="next",type="Macro"
  Cil_starg_s,       // name="starg.s",input="Pop1",output="Push0",args="ShortInlineVar",o1="0xFF",o2="0x10",flow="next",type="Macro"
  Cil_ldloc_s,       // name="ldloc.s",input="Pop0",output="Push1",args="ShortInlineVar",o1="0xFF",o2="0x11",flow="next",type="Macro"
  Cil_ldloca_s,      // name="ldloca.s",input="Pop0",output="PushI",args="ShortInlineVar",o1="0xFF",o2="0x12",flow="next",type="Macro"
  Cil_stloc_s,       // name="stloc.s",input="Pop1",output="Push0",args="ShortInlineVar",o1="0xFF",o2="0x13",flow="next",type="Macro"
  Cil_ldnull,        // name="ldnull",input="Pop0",output="PushRef",args="InlineNone",o1="0xFF",o2="0x14",flow="next",type="Primitive"
  Cil_ldc_i4_m1,     // name="ldc.i4.m1",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x15",flow="next",type="Macro",constant="-1"
  Cil_ldc_i4_0,      // name="ldc.i4.0",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x16",flow="next",type="Macro",constant="0"
  Cil_ldc_i4_1,      // name="ldc.i4.1",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x17",flow="next",type="Macro",constant="1"
  Cil_ldc_i4_2,      // name="ldc.i4.2",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x18",flow="next",type="Macro",constant="2"
  Cil_ldc_i4_3,      // name="ldc.i4.3",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x19",flow="next",type="Macro",constant="3"
  Cil_ldc_i4_4,      // name="ldc.i4.4",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1A",flow="next",type="Macro",constant="4"
  Cil_ldc_i4_5,      // name="ldc.i4.5",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1B",flow="next",type="Macro",constant="5"
  Cil_ldc_i4_6,      // name="ldc.i4.6",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1C",flow="next",type="Macro",constant="6"
  Cil_ldc_i4_7,      // name="ldc.i4.7",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1D",flow="next",type="Macro",constant="7"
  Cil_ldc_i4_8,      // name="ldc.i4.8",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1E",flow="next",type="Macro",constant="8"
  Cil_ldc_i4_s,      // name="ldc.i4.s",input="Pop0",output="PushI",args="ShortInlineI",o1="0xFF",o2="0x1F",flow="next",type="Macro"
  Cil_ldc_i4,        // name="ldc.i4",input="Pop0",output="PushI",args="InlineI",o1="0xFF",o2="0x20",flow="next",type="Primitive"
  Cil_ldc_i8,        // name="ldc.i8",input="Pop0",output="PushI8",args="InlineI8",o1="0xFF",o2="0x21",flow="next",type="Primitive"
  Cil_ldc_r4,        // name="ldc.r4",input="Pop0",output="PushR4",args="ShortInlineR",o1="0xFF",o2="0x22",flow="next",type="Primitive"
  Cil_ldc_r8,        // name="ldc.r8",input="Pop0",output="PushR8",args="InlineR",o1="0xFF",o2="0x23",flow="next",type="Primitive"
  Cil_unused99,      // name="unused99",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x24",flow="next"
  Cil_dup,           // name="dup",input="Pop1",output="Push1+Push1",args="InlineNone",o1="0xFF",o2="0x25",flow="next",type="Primitive"
  Cil_pop,           // name="pop",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x26",flow="next",type="Primitive"
  Cil_jmp,           // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call",type="Primitive"
  Cil_call,          // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call",type="Primitive"
  Cil_calli,         // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call",type="Primitive"
  Cil_ret,           // name="ret",input="VarPop",output="Push0",args="InlineNone",o1="0xFF",o2="0x2A",flow="return",type="Primitive"
  Cil_br_s,          // name="br.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2B",flow="branch",type="Macro"
  Cil_brfalse_s,     // name="brfalse.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2C",flow="cond-branch",type="Macro"
  Cil_brtrue_s,      // name="brtrue.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2D",flow="cond-branch",type="Macro"
  Cil_beq_s,         // name="beq.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2E",flow="cond-branch",type="Macro"
  Cil_bge_s,         // name="bge.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2F",flow="cond-branch",type="Macro"
  Cil_bgt_s,         // name="bgt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x30",flow="cond-branch",type="Macro"
  Cil_ble_s,         // name="ble.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x31",flow="cond-branch",type="Macro"
  Cil_blt_s,         // name="blt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x32",flow="cond-branch",type="Macro"
  Cil_bne_un_s,      // name="bne.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x33",flow="cond-branch",type="Macro"
  Cil_bge_un_s,      // name="bge.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x34",flow="cond-branch",type="Macro"
  Cil_bgt_un_s,      // name="bgt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x35",flow="cond-branch",type="Macro"
  Cil_ble_un_s,      // name="ble.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x36",flow="cond-branch",type="Macro"
  Cil_blt_un_s,      // name="blt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x37",flow="cond-branch",type="Macro"
  Cil_br,            // name="br",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x38",flow="branch",type="Primitive"
  Cil_brfalse,       // name="brfalse",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x39",flow="cond-branch",type="Primitive"
  Cil_brtrue,        // name="brtrue",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3A",flow="cond-branch",type="Primitive"
  Cil_beq,           // name="beq",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3B",flow="cond-branch",type="Macro"
  Cil_bge,           // name="bge",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3C",flow="cond-branch",type="Macro"
  Cil_bgt,           // name="bgt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3D",flow="cond-branch",type="Macro"
  Cil_ble,           // name="ble",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3E",flow="cond-branch",type="Macro"
  Cil_blt,           // name="blt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3F",flow="cond-branch",type="Macro"
  Cil_bne_un,        // name="bne.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x40",flow="cond-branch",type="Macro"
  Cil_bge_un,        // name="bge.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x41",flow="cond-branch",type="Macro"
  Cil_bgt_un,        // name="bgt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x42",flow="cond-branch",type="Macro"
  Cil_ble_un,        // name="ble.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x43",flow="cond-branch",type="Macro"
  Cil_blt_un,        // name="blt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x44",flow="cond-branch",type="Macro"
  Cil_switch,        // name="switch",input="PopI",output="Push0",args="InlineSwitch",o1="0xFF",o2="0x45",flow="cond-branch",type="Primitive"
  Cil_ldind_i1,      // name="ldind.i1",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x46",flow="next",type="Primitive"
  Cil_ldind_u1,      // name="ldind.u1",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x47",flow="next",type="Primitive"
  Cil_ldind_i2,      // name="ldind.i2",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x48",flow="next",type="Primitive"
  Cil_ldind_u2,      // name="ldind.u2",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x49",flow="next",type="Primitive"
  Cil_ldind_i4,      // name="ldind.i4",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x4A",flow="next",type="Primitive"
  Cil_ldind_u4,      // name="ldind.u4",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x4B",flow="next",type="Primitive"
  Cil_ldind_i8,      // name="ldind.i8",input="PopI",output="PushI8",args="InlineNone",o1="0xFF",o2="0x4C",flow="next",type="Primitive"

  // DQ (11/6/2021): Added this enoum missed in the automated mechanism done by Craig.
  Cil_ldind_u8,      // name="ldind.i8",input="PopI",output="PushI8",args="InlineNone",o1="0xFF",o2="0x4C",flow="next",type="Primitive"

  Cil_ldind_i,       // name="ldind.i",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x4D",flow="next",type="Primitive"
  Cil_ldind_r4,      // name="ldind.r4",input="PopI",output="PushR4",args="InlineNone",o1="0xFF",o2="0x4E",flow="next",type="Primitive"
  Cil_ldind_r8,      // name="ldind.r8",input="PopI",output="PushR8",args="InlineNone",o1="0xFF",o2="0x4F",flow="next",type="Primitive"
  Cil_ldind_ref,     // name="ldind.ref",input="PopI",output="PushRef",args="InlineNone",o1="0xFF",o2="0x50",flow="next",type="Primitive"
  Cil_stind_ref,     // name="stind.ref",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x51",flow="next",type="Primitive"
  Cil_stind_i1,      // name="stind.i1",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x52",flow="next",type="Primitive"
  Cil_stind_i2,      // name="stind.i2",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x53",flow="next",type="Primitive"
  Cil_stind_i4,      // name="stind.i4",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x54",flow="next",type="Primitive"
  Cil_stind_i8,      // name="stind.i8",input="PopI+PopI8",output="Push0",args="InlineNone",o1="0xFF",o2="0x55",flow="next",type="Primitive"
  Cil_stind_r4,      // name="stind.r4",input="PopI+PopR4",output="Push0",args="InlineNone",o1="0xFF",o2="0x56",flow="next",type="Primitive"
  Cil_stind_r8,      // name="stind.r8",input="PopI+PopR8",output="Push0",args="InlineNone",o1="0xFF",o2="0x57",flow="next",type="Primitive"
  Cil_add,           // name="add",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x58",flow="next",type="Primitive"
  Cil_sub,           // name="sub",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x59",flow="next",type="Primitive"
  Cil_mul,           // name="mul",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5A",flow="next",type="Primitive"
  Cil_div,           // name="div",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5B",flow="next",type="Primitive"
  Cil_div_un,        // name="div.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5C",flow="next",type="Primitive"
  Cil_rem,           // name="rem",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5D",flow="next",type="Primitive"
  Cil_rem_un,        // name="rem.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5E",flow="next",type="Primitive"
  Cil_and,           // name="and",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5F",flow="next",type="Primitive"
  Cil_or,            // name="or",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x60",flow="next",type="Primitive"
  Cil_xor,           // name="xor",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x61",flow="next",type="Primitive"
  Cil_shl,           // name="shl",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x62",flow="next",type="Primitive"
  Cil_shr,           // name="shr",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x63",flow="next",type="Primitive"
  Cil_shr_un,        // name="shr.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x64",flow="next",type="Primitive"
  Cil_neg,           // name="neg",input="Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x65",flow="next",type="Primitive"
  Cil_not,           // name="not",input="Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x66",flow="next",type="Primitive"
  Cil_conv_i1,       // name="conv.i1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x67",flow="next",type="Primitive"
  Cil_conv_i2,       // name="conv.i2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x68",flow="next",type="Primitive"
  Cil_conv_i4,       // name="conv.i4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x69",flow="next",type="Primitive"
  Cil_conv_i8,       // name="conv.i8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x6A",flow="next",type="Primitive"
  Cil_conv_r4,       // name="conv.r4",input="Pop1",output="PushR4",args="InlineNone",o1="0xFF",o2="0x6B",flow="next",type="Primitive"
  Cil_conv_r8,       // name="conv.r8",input="Pop1",output="PushR8",args="InlineNone",o1="0xFF",o2="0x6C",flow="next",type="Primitive"
  Cil_conv_u4,       // name="conv.u4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x6D",flow="next",type="Primitive"
  Cil_conv_u8,       // name="conv.u8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x6E",flow="next",type="Primitive"
  Cil_callvirt,      // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call",type="Objmodel"
  Cil_cpobj,         // name="cpobj",input="PopI+PopI",output="Push0",args="InlineType",o1="0xFF",o2="0x70",flow="next",type="Objmodel"
  Cil_ldobj,         // name="ldobj",input="PopI",output="Push1",args="InlineType",o1="0xFF",o2="0x71",flow="next",type="Objmodel"
  Cil_ldstr,         // name="ldstr",input="Pop0",output="PushRef",args="InlineString",o1="0xFF",o2="0x72",flow="next",type="Objmodel"
  Cil_newobj,        // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call",type="Objmodel"
  Cil_castclass,     // name="castclass",input="PopRef",output="PushRef",args="InlineType",o1="0xFF",o2="0x74",flow="next",type="Objmodel"
  Cil_isinst,        // name="isinst",input="PopRef",output="PushI",args="InlineType",o1="0xFF",o2="0x75",flow="next",type="Objmodel"
  Cil_conv_r_un,     // name="conv.r.un",input="Pop1",output="PushR8",args="InlineNone",o1="0xFF",o2="0x76",flow="next",type="Primitive"
  Cil_unused58,      // name="unused58",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x77",flow="next"
  Cil_unused1,       // name="unused1",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x78",flow="next"
  Cil_unbox,         // name="unbox",input="PopRef",output="PushI",args="InlineType",o1="0xFF",o2="0x79",flow="next",type="Primitive"
  Cil_throw,         // name="throw",input="PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0x7A",flow="throw",type="Objmodel"
  Cil_ldfld,         // name="ldfld",input="PopRef",output="Push1",args="InlineField",o1="0xFF",o2="0x7B",flow="next",type="Objmodel"
  Cil_ldflda,        // name="ldflda",input="PopRef",output="PushI",args="InlineField",o1="0xFF",o2="0x7C",flow="next",type="Objmodel"
  Cil_stfld,         // name="stfld",input="PopRef+Pop1",output="Push0",args="InlineField",o1="0xFF",o2="0x7D",flow="next",type="Objmodel"
  Cil_ldsfld,        // name="ldsfld",input="Pop0",output="Push1",args="InlineField",o1="0xFF",o2="0x7E",flow="next",type="Objmodel"
  Cil_ldsflda,       // name="ldsflda",input="Pop0",output="PushI",args="InlineField",o1="0xFF",o2="0x7F",flow="next",type="Objmodel"
  Cil_stsfld,        // name="stsfld",input="Pop1",output="Push0",args="InlineField",o1="0xFF",o2="0x80",flow="next",type="Objmodel"
  Cil_stobj,         // name="stobj",input="PopI+Pop1",output="Push0",args="InlineType",o1="0xFF",o2="0x81",flow="next",type="Primitive"
  Cil_conv_ovf_i1_un,// name="conv.ovf.i1.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x82",flow="next",type="Primitive"
  Cil_conv_ovf_i2_un,// name="conv.ovf.i2.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x83",flow="next",type="Primitive"
  Cil_conv_ovf_i4_un,// name="conv.ovf.i4.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x84",flow="next",type="Primitive"
  Cil_conv_ovf_i8_un,// name="conv.ovf.i8.un",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x85",flow="next",type="Primitive"
  Cil_conv_ovf_u1_un,// name="conv.ovf.u1.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x86",flow="next",type="Primitive"
  Cil_conv_ovf_u2_un,// name="conv.ovf.u2.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x87",flow="next",type="Primitive"
  Cil_conv_ovf_u4_un,// name="conv.ovf.u4.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x88",flow="next",type="Primitive"
  Cil_conv_ovf_u8_un,// name="conv.ovf.u8.un",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x89",flow="next",type="Primitive"
  Cil_conv_ovf_i_un, // name="conv.ovf.i.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x8A",flow="next",type="Primitive"
  Cil_conv_ovf_u_un, // name="conv.ovf.u.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x8B",flow="next",type="Primitive"
  Cil_box,           // name="box",input="Pop1",output="PushRef",args="InlineType",o1="0xFF",o2="0x8C",flow="next",type="Primitive"
  Cil_newarr,        // name="newarr",input="PopI",output="PushRef",args="InlineType",o1="0xFF",o2="0x8D",flow="next",type="Objmodel"
  Cil_ldlen,         // name="ldlen",input="PopRef",output="PushI",args="InlineNone",o1="0xFF",o2="0x8E",flow="next",type="Objmodel"
  Cil_ldelema,       // name="ldelema",input="PopRef+PopI",output="PushI",args="InlineType",o1="0xFF",o2="0x8F",flow="next",type="Objmodel"
  Cil_ldelem_i1,     // name="ldelem.i1",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x90",flow="next",type="Objmodel"
  Cil_ldelem_u1,     // name="ldelem.u1",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x91",flow="next",type="Objmodel"
  Cil_ldelem_i2,     // name="ldelem.i2",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x92",flow="next",type="Objmodel"
  Cil_ldelem_u2,     // name="ldelem.u2",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x93",flow="next",type="Objmodel"
  Cil_ldelem_i4,     // name="ldelem.i4",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x94",flow="next",type="Objmodel"
  Cil_ldelem_u4,     // name="ldelem.u4",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x95",flow="next",type="Objmodel"
  Cil_ldelem_i8,     // name="ldelem.i8",input="PopRef+PopI",output="PushI8",args="InlineNone",o1="0xFF",o2="0x96",flow="next",type="Objmodel"
  Cil_ldelem_i,      // name="ldelem.i",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x97",flow="next",type="Objmodel"
  Cil_ldelem_r4,     // name="ldelem.r4",input="PopRef+PopI",output="PushR4",args="InlineNone",o1="0xFF",o2="0x98",flow="next",type="Objmodel"
  Cil_ldelem_r8,     // name="ldelem.r8",input="PopRef+PopI",output="PushR8",args="InlineNone",o1="0xFF",o2="0x99",flow="next",type="Objmodel"
  Cil_ldelem_ref,    // name="ldelem.ref",input="PopRef+PopI",output="PushRef",args="InlineNone",o1="0xFF",o2="0x9A",flow="next",type="Objmodel"
  Cil_stelem_i,      // name="stelem.i",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9B",flow="next",type="Objmodel"
  Cil_stelem_i1,     // name="stelem.i1",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9C",flow="next",type="Objmodel"
  Cil_stelem_i2,     // name="stelem.i2",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9D",flow="next",type="Objmodel"
  Cil_stelem_i4,     // name="stelem.i4",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9E",flow="next",type="Objmodel"
  Cil_stelem_i8,     // name="stelem.i8",input="PopRef+PopI+PopI8",output="Push0",args="InlineNone",o1="0xFF",o2="0x9F",flow="next",type="Objmodel"
  Cil_stelem_r4,     // name="stelem.r4",input="PopRef+PopI+PopR4",output="Push0",args="InlineNone",o1="0xFF",o2="0xA0",flow="next",type="Objmodel"
  Cil_stelem_r8,     // name="stelem.r8",input="PopRef+PopI+PopR8",output="Push0",args="InlineNone",o1="0xFF",o2="0xA1",flow="next",type="Objmodel"
  Cil_stelem_ref,    // name="stelem.ref",input="PopRef+PopI+PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0xA2",flow="next",type="Objmodel"
  Cil_ldelem,        // name="ldelem",input="PopRef+PopI",output="Push1",args="InlineType",o1="0xFF",o2="0xA3",flow="next",type="Objmodel"
  Cil_stelem,        // name="stelem",input="PopRef+PopI+Pop1",output="Push0",args="InlineType",o1="0xFF",o2="0xA4",flow="next",type="Objmodel"
  Cil_unbox_any,     // name="unbox.any",input="PopRef",output="Push1",args="InlineType",o1="0xFF",o2="0xA5",flow="next",type="Objmodel"
  Cil_unused5,       // name="unused5",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA6",flow="next"
  Cil_unused6,       // name="unused6",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA7",flow="next"
  Cil_unused7,       // name="unused7",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA8",flow="next"
  Cil_unused8,       // name="unused8",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA9",flow="next"
  Cil_unused9,       // name="unused9",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAA",flow="next"
  Cil_unused10,      // name="unused10",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAB",flow="next"
  Cil_unused11,      // name="unused11",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAC",flow="next"
  Cil_unused12,      // name="unused12",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAD",flow="next"
  Cil_unused13,      // name="unused13",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAE",flow="next"
  Cil_unused14,      // name="unused14",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAF",flow="next"
  Cil_unused15,      // name="unused15",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xB0",flow="next"
  Cil_unused16,      // name="unused16",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xB1",flow="next"
  Cil_unused17,      // name="unused17",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xB2",flow="next"
  Cil_conv_ovf_i1,   // name="conv.ovf.i1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB3",flow="next",type="Primitive"
  Cil_conv_ovf_u1,   // name="conv.ovf.u1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB4",flow="next",type="Primitive"
  Cil_conv_ovf_i2,   // name="conv.ovf.i2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB5",flow="next",type="Primitive"
  Cil_conv_ovf_u2,   // name="conv.ovf.u2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB6",flow="next",type="Primitive"
  Cil_conv_ovf_i4,   // name="conv.ovf.i4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB7",flow="next",type="Primitive"
  Cil_conv_ovf_u4,   // name="conv.ovf.u4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB8",flow="next",type="Primitive"
  Cil_conv_ovf_i8,   // name="conv.ovf.i8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0xB9",flow="next",type="Primitive"
  Cil_conv_ovf_u8,   // name="conv.ovf.u8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0xBA",flow="next",type="Primitive"
  Cil_unused50,      // name="unused50",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBB",flow="next"
  Cil_unused18,      // name="unused18",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBC",flow="next"
  Cil_unused19,      // name="unused19",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBD",flow="next"
  Cil_unused20,      // name="unused20",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBE",flow="next"
  Cil_unused21,      // name="unused21",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBF",flow="next"
  Cil_unused22,      // name="unused22",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC0",flow="next"
  Cil_unused23,      // name="unused23",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC1",flow="next"
  Cil_refanyval,     // name="refanyval",input="Pop1",output="PushI",args="InlineType",o1="0xFF",o2="0xC2",flow="next",type="Primitive"
  Cil_ckfinite,      // name="ckfinite",input="Pop1",output="PushR8",args="InlineNone",o1="0xFF",o2="0xC3",flow="next",type="Primitive"
  Cil_unused24,      // name="unused24",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC4",flow="next"
  Cil_unused25,      // name="unused25",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC5",flow="next"
  Cil_mkrefany,      // name="mkrefany",input="PopI",output="Push1",args="InlineType",o1="0xFF",o2="0xC6",flow="next",type="Primitive"
  Cil_unused59,      // name="unused59",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC7",flow="next"
  Cil_unused60,      // name="unused60",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC8",flow="next"
  Cil_unused61,      // name="unused61",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC9",flow="next"
  Cil_unused62,      // name="unused62",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCA",flow="next"
  Cil_unused63,      // name="unused63",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCB",flow="next"
  Cil_unused64,      // name="unused64",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCC",flow="next"
  Cil_unused65,      // name="unused65",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCD",flow="next"
  Cil_unused66,      // name="unused66",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCE",flow="next"
  Cil_unused67,      // name="unused67",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCF",flow="next"
  Cil_ldtoken,       // name="ldtoken",input="Pop0",output="PushI",args="InlineTok",o1="0xFF",o2="0xD0",flow="next",type="Primitive"
  Cil_conv_u2,       // name="conv.u2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD1",flow="next",type="Primitive"
  Cil_conv_u1,       // name="conv.u1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD2",flow="next",type="Primitive"
  Cil_conv_i,        // name="conv.i",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD3",flow="next",type="Primitive"
  Cil_conv_ovf_i,    // name="conv.ovf.i",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD4",flow="next",type="Primitive"
  Cil_conv_ovf_u,    // name="conv.ovf.u",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD5",flow="next",type="Primitive"
  Cil_add_ovf,       // name="add.ovf",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD6",flow="next",type="Primitive"
  Cil_add_ovf_un,    // name="add.ovf.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD7",flow="next",type="Primitive"
  Cil_mul_ovf,       // name="mul.ovf",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD8",flow="next",type="Primitive"
  Cil_mul_ovf_un,    // name="mul.ovf.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD9",flow="next",type="Primitive"
  Cil_sub_ovf,       // name="sub.ovf",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xDA",flow="next",type="Primitive"
  Cil_sub_ovf_un,    // name="sub.ovf.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xDB",flow="next",type="Primitive"
  Cil_endfinally,    // name="endfinally",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xDC",flow="return",type="Primitive"
  Cil_leave,         // name="leave",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0xDD",flow="branch",type="Primitive"
  Cil_leave_s,       // name="leave.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0xDE",flow="branch",type="Primitive"
  Cil_stind_i,       // name="stind.i",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0xDF",flow="next",type="Primitive"
  Cil_conv_u,        // name="conv.u",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xE0",flow="next",type="Primitive"
  Cil_unused26,      // name="unused26",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE1",flow="next"
  Cil_unused27,      // name="unused27",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE2",flow="next"
  Cil_unused28,      // name="unused28",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE3",flow="next"
  Cil_unused29,      // name="unused29",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE4",flow="next"
  Cil_unused30,      // name="unused30",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE5",flow="next"
  Cil_unused31,      // name="unused31",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE6",flow="next"
  Cil_unused32,      // name="unused32",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE7",flow="next"
  Cil_unused33,      // name="unused33",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE8",flow="next"
  Cil_unused34,      // name="unused34",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE9",flow="next"
  Cil_unused35,      // name="unused35",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEA",flow="next"
  Cil_unused36,      // name="unused36",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEB",flow="next"
  Cil_unused37,      // name="unused37",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEC",flow="next"
  Cil_unused38,      // name="unused38",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xED",flow="next"
  Cil_unused39,      // name="unused39",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEE",flow="next"
  Cil_unused40,      // name="unused40",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEF",flow="next"
  Cil_unused41,      // name="unused41",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF0",flow="next"
  Cil_unused42,      // name="unused42",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF1",flow="next"
  Cil_unused43,      // name="unused43",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF2",flow="next"
  Cil_unused44,      // name="unused44",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF3",flow="next"
  Cil_unused45,      // name="unused45",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF4",flow="next"
  Cil_unused46,      // name="unused46",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF5",flow="next"
  Cil_unused47,      // name="unused47",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF6",flow="next"
  Cil_unused48,      // name="unused48",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF7",flow="next"
  Cil_prefix7,       // name="prefix7",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF8",flow="meta",type="Nternal"
  Cil_prefix6,       // name="prefix6",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF9",flow="meta",type="Nternal"
  Cil_prefix5,       // name="prefix5",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFA",flow="meta",type="Nternal"
  Cil_prefix4,       // name="prefix4",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFB",flow="meta",type="Nternal"
  Cil_prefix3,       // name="prefix3",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFC",flow="meta",type="Nternal"
  Cil_prefix2,       // name="prefix2",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFD",flow="meta",type="Nternal"
  Cil_prefix1,       // name="prefix1",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFE",flow="meta",type="Nternal"
  Cil_prefixref,     // name="prefixref",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFF",flow="meta",type="Nternal"
  Cil_arglist,       // name="arglist",input="Pop0",output="PushI",args="InlineNone",o1="0xFE",o2="0x00",flow="next",type="Primitive"
  Cil_ceq,           // name="ceq",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x01",flow="next",type="Primitive"
  Cil_cgt,           // name="cgt",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x02",flow="next",type="Primitive"
  Cil_cgt_un,        // name="cgt.un",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x03",flow="next",type="Primitive"
  Cil_clt,           // name="clt",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x04",flow="next",type="Primitive"
  Cil_clt_un,        // name="clt.un",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x05",flow="next",type="Primitive"
  Cil_ldftn,         // name="ldftn",input="Pop0",output="PushI",args="InlineMethod",o1="0xFE",o2="0x06",flow="next",type="Primitive"
  Cil_ldvirtftn,     // name="ldvirtftn",input="PopRef",output="PushI",args="InlineMethod",o1="0xFE",o2="0x07",flow="next",type="Primitive"
  Cil_unused56,      // name="unused56",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x08",flow="next"
  Cil_ldarg,         // name="ldarg",input="Pop0",output="Push1",args="InlineVar",o1="0xFE",o2="0x09",flow="next",type="Primitive"
  Cil_ldarga,        // name="ldarga",input="Pop0",output="PushI",args="InlineVar",o1="0xFE",o2="0x0A",flow="next",type="Primitive"
  Cil_starg,         // name="starg",input="Pop1",output="Push0",args="InlineVar",o1="0xFE",o2="0x0B",flow="next",type="Primitive"
  Cil_ldloc,         // name="ldloc",input="Pop0",output="Push1",args="InlineVar",o1="0xFE",o2="0x0C",flow="next",type="Primitive"
  Cil_ldloca,        // name="ldloca",input="Pop0",output="PushI",args="InlineVar",o1="0xFE",o2="0x0D",flow="next",type="Primitive"
  Cil_stloc,         // name="stloc",input="Pop1",output="Push0",args="InlineVar",o1="0xFE",o2="0x0E",flow="next",type="Primitive"
  Cil_localloc,      // name="localloc",input="PopI",output="PushI",args="InlineNone",o1="0xFE",o2="0x0F",flow="next",type="Primitive"
  Cil_unused57,      // name="unused57",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x10",flow="next"
  Cil_endfilter,     // name="endfilter",input="PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x11",flow="return",type="Primitive"
  Cil_unaligned_,    // name="unaligned.",input="Pop0",output="Push0",args="ShortInlineI",o1="0xFE",o2="0x12",flow="meta",type="Prefix"
  Cil_volatile_,     // name="volatile.",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x13",flow="meta",type="Prefix"
  Cil_tail_,         // name="tail.",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x14",flow="meta",type="Prefix"
  Cil_initobj,       // name="initobj",input="PopI",output="Push0",args="InlineType",o1="0xFE",o2="0x15",flow="next",type="Objmodel"
  Cil_constrained_,  // name="constrained.",input="Pop0",output="Push0",args="InlineType",o1="0xFE",o2="0x16",flow="meta",type="Prefix"
  Cil_cpblk,         // name="cpblk",input="PopI+PopI+PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x17",flow="next",type="Primitive"
  Cil_initblk,       // name="initblk",input="PopI+PopI+PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x18",flow="next",type="Primitive"
  Cil_no_,           // name="no.",input="Pop0",output="Push0",args="ShortInlineI",o1="0xFE",o2="0x19",flow="next"
  Cil_rethrow,       // name="rethrow",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1A",flow="throw",type="Objmodel"
  Cil_unused,        // name="unused",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1B",flow="next"
  Cil_sizeof,        // name="sizeof",input="Pop0",output="PushI",args="InlineType",o1="0xFE",o2="0x1C",flow="next",type="Primitive"
  Cil_refanytype,    // name="refanytype",input="Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x1D",flow="next",type="Primitive"
  Cil_readonly_,     // name="readonly.",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1E",flow="meta"
  Cil_unused53,      // name="unused53",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1F",flow="next"
  Cil_unused54,      // name="unused54",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x20",flow="next"
  Cil_unused55,      // name="unused55",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x21",flow="next"
  Cil_unused70,      // name="unused70",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x22",flow="next"
  Cil_illegal,       // name="illegal",input="Pop0",output="Push0",args="InlineNone",o1="0x00",o2="0x00",flow="meta"
  Cil_endmac,        // name="endmac",input="Pop0",output="Push0",args="InlineNone",o1="0x00",o2="0x00",flow="meta"
  Cil_mono_icall,    // name="mono_icall",input="VarPop",output="VarPush",args="InlineI",o1="0xF0",o2="0x00",flow="next"
  Cil_mono_objaddr,  // name="mono_objaddr",input="Pop1",output="PushI",args="InlineNone",o1="0xF0",o2="0x01",flow="next"
  Cil_mono_ldptr,    // name="mono_ldptr",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x02",flow="next"
  Cil_mono_vtaddr,   // name="mono_vtaddr",input="Pop1",output="PushI",args="InlineNone",o1="0xF0",o2="0x03",flow="next"
  Cil_mono_newobj,   // name="mono_newobj",input="Pop0",output="PushRef",args="InlineType",o1="0xF0",o2="0x04",flow="next"
  Cil_mono_retobj,   // name="mono_retobj",input="PopI",output="Push0",args="InlineType",o1="0xF0",o2="0x05",flow="return"
  Cil_mono_ldnativeobj,// name="mono_ldnativeobj",input="PopI",output="Push1",args="InlineType",o1="0xF0",o2="0x06",flow="return"
  Cil_mono_cisinst,  // name="mono_cisinst",input="PopRef",output="Push1",args="InlineType",o1="0xF0",o2="0x07",flow="next"
  Cil_mono_ccastclass,// name="mono_ccastclass",input="PopRef",output="Push1",args="InlineType",o1="0xF0",o2="0x08",flow="next"
  Cil_mono_save_lmf, // name="mono_save_lmf",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x09",flow="next"
  Cil_mono_restore_lmf,// name="mono_restore_lmf",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x0A",flow="next"
  Cil_mono_classconst,// name="mono_classconst",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x0B",flow="next"
  Cil_mono_not_taken,// name="mono_not_taken",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x0C",flow="next"
  Cil_mono_tls,      // name="mono_tls",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x0D",flow="next"
  Cil_mono_icall_addr,// name="mono_icall_addr",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x0E",flow="next"
  Cil_mono_dyn_call, // name="mono_dyn_call",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x0F",flow="next"
  Cil_mono_memory_barrier,// name="mono_memory_barrier",input="Pop0",output="Push0",args="InlineI",o1="0xF0",o2="0x10",flow="next"
  Cil_unused71,      // name="unused71",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x11",flow="next"
  Cil_unused72,      // name="unused72",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x12",flow="next"
  Cil_mono_jit_icall_addr,// name="mono_jit_icall_addr",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x13",flow="next"
  Cil_mono_ldptr_int_req_flag,// name="mono_ldptr_int_req_flag",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x14",flow="next"
  Cil_mono_ldptr_card_table,// name="mono_ldptr_card_table",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x15",flow="next"
  Cil_mono_ldptr_nursery_start,// name="mono_ldptr_nursery_start",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x16",flow="next"
  Cil_mono_ldptr_nursery_bits,// name="mono_ldptr_nursery_bits",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x17",flow="next"
  Cil_mono_calli_extra_arg,// name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"
  Cil_mono_lddomain, // name="mono_lddomain",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x19",flow="next"
  Cil_mono_atomic_store_i4,// name="mono_atomic_store_i4",input="PopI+PopI",output="Push0",args="InlineI",o1="0xF0",o2="0x1A",flow="next"
  Cil_mono_save_last_error,// name="mono_save_last_error",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x1B",flow="next"
  Cil_mono_get_rgctx_arg,// name="mono_get_rgctx_arg",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x1C",flow="next"
  Cil_mono_ldptr_profiler_allocation_count,// name="mono_ldptr_profiler_allocation_count",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x1D",flow="next"
  Cil_mono_ld_delegate_method_ptr,// name="mono_ld_delegate_method_ptr",input="Pop1",output="PushI",args="InlineNone",o1="0xF0",o2="0x1E",flow="next"
  Cil_mono_rethrow,  // name="mono_rethrow",input="PopRef",output="Push0",args="InlineNone",o1="0xF0",o2="0x1F",flow="throw",type="Objmodel"
  Cil_mono_get_sp,   // name="mono_get_sp",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x20",flow="next"
  Cil_mono_methodconst,// name="mono_methodconst",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x21",flow="next"
  Cil_mono_pinvoke_addr_cache,// name="mono_pinvoke_addr_cache",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x22",flow="next"

  // DQ (11/72021): Added prefixes for instructions (need to check if this is the best way to support these):
  Cil_constrained,
  Cil_no,
  Cil_readonly,
  Cil_tail,
  Cil_unaligned,
  Cil_volatile,

  // This must be last
  Cil_last_instruction
}; // End CilInstructionKind

} // namespace
} // namespace

#endif
#endif
