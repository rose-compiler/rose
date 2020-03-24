/* Assembly specific to the x86 architecture. */

#ifndef ROSE_BinaryAnalysis_AssemblerX86_H
#define ROSE_BinaryAnalysis_AssemblerX86_H

#include "Assembler.h"
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {

/** This class contains methods for assembling x86 instructions (SgAsmX86Instruction).
 *
 *  End users will generally not need to use the AssemblerX86 class directly. Instead, they will call Assembler::create() to
 *  create an assembler that's appropriate for a particular binary file header or interpretation and then use that assembler to
 *  assemble instructions.
 *
 *  The assembler itself is quite small compared to the disassembler (about one third the size) and doesn't actually know about
 *  any instructions; it only knows how to encode various prefixes and operand addressing modes.  For each instruction to be
 *  assembled, the assembler consults a dictionary of assembly definitions. The instruction is looked up in this dictionary and
 *  the chosen definition then drives the assembly. If the instruction being assembled matches multiple definitions then each
 *  valid definition is tried and the "best" one (see Assembler::set_encoding_type()) is returned.
 *
 *  The dictionary is generated directly from the Intel "Instruction Set Reference" PDF documentation as augmented by a small
 *  text file in this directory. The IntelAssemblyBuilder perl script generates AssemblerX86Init.h and AssemblerX86Init.C, which
 *  contain the X86InstructionKind enumeration, a function to initialize the dictionary (AssemblerX86::initAssemblyRules()),
 *  and a function for converting an X86InstructionKind constant to a string (AssemblerX86::to_str()). */
class AssemblerX86: public Assembler {
public:
    AssemblerX86()
        : honor_operand_types(false) {
        if (defns.size()==0)
            initAssemblyRules();
    }

    virtual ~AssemblerX86() {}

    /** Assemble an instruction (SgAsmInstruction) into byte code. The new bytes are added to the end of the vector. */
    virtual SgUnsignedCharList assembleOne(SgAsmInstruction*);
    
    /** Causes the assembler to honor (if true) or disregard (if false) the data types of operands when assembling. For
     *  instance, when honoring operand data types, if an operand is a 32-bit SgAsmIntegerValueExpression then the assembler
     *  will attempt to encode it as four bytes even if its value could be encoded as a single byte. This is turned on
     *  automatically if the Assembler::set_encoding_type() is set to Assembler::ET_MATCHES, but can also be turned on
     *  independently. */
    void set_honor_operand_types(bool b) {
        honor_operand_types = b;
    }

    /** Returns true if the assembler is honoring operand data types, or false if the assembler is using the smallest possible
     *  encoding. */
    bool get_honor_operand_types() const {
        return honor_operand_types;
    }

    /** Assemble an x86 program from assembly source code using the nasm assembler. */
    virtual SgUnsignedCharList assembleProgram(const std::string &source);

    /*========================================================================================================================
     * Members for defining instructions.
     *========================================================================================================================*/
private:
    /* These bit masks specify how the opcode part of the encoding is generated. The base opcode bytes are specified with
     * a 64-bit value so that up to eight bytes of opcode can be specified.  The bytes generated come from the 64-bit opcode
     * value in big-endian order but without leading zero bytes. If the 64-bit opcode is zero then a single zero byte is
     * generated.  For instance, the MONITOR instruction has an opcode of 0x0f01c8, generating the encoding 0x0f, 0x01, 0xc8. */
    
    /** Indicates that the ModR/M byte of the instruction uses only the r/m (register or memory) operand. The reg field
     *  contains @p n, providing an extension to the instruction's opcode. This form is written as "/0", "/1", etc. in the
     *  Intel documentation. */
    static const unsigned od_e_mask   = 0x00000070;                /* mask for numeric value (n) part of En field. */
    static const unsigned od_e_pres   = 0x00000080;                /* bit set if En modification was specified. */
    static const unsigned od_e0       = 0x00000000 | od_e_pres;
    static const unsigned od_e1       = 0x00000010 | od_e_pres;
    static const unsigned od_e2       = 0x00000020 | od_e_pres;
    static const unsigned od_e3       = 0x00000030 | od_e_pres;
    static const unsigned od_e4       = 0x00000040 | od_e_pres;
    static const unsigned od_e5       = 0x00000050 | od_e_pres;
    static const unsigned od_e6       = 0x00000060 | od_e_pres;
    static const unsigned od_e7       = 0x00000070 | od_e_pres;
    static size_t od_e_val(unsigned opcode_mods) { return (opcode_mods & od_e_mask)>>4; } /**< Returns value of En modification. */

    /** Indicates the use of a REX prefix that affects operand size or instruction semantics. The ordering of the REX prefix
     *  and other optional/mandatory instruction prefixes are discussed in Chapter 2 of the Intel "Instruction Set Reference,
     *  A-M". */
    static const unsigned od_rex_pres = 0x00000001;                /* bit set if REX prefix is present. */
    static const unsigned od_rex_mask = 0x00000f00;                /* mask for low nyble of REX byte. */
    static const unsigned od_rex      = 0x00000000 | od_rex_pres;
    static const unsigned od_rexb     = 0x00000100 | od_rex_pres;
    static const unsigned od_rexx     = 0x00000200 | od_rex_pres;
    static const unsigned od_rexxb    = 0x00000300 | od_rex_pres;
    static const unsigned od_rexr     = 0x00000400 | od_rex_pres;
    static const unsigned od_rexrb    = 0x00000500 | od_rex_pres;
    static const unsigned od_rexrx    = 0x00000600 | od_rex_pres;
    static const unsigned od_rexrxb   = 0x00000700 | od_rex_pres;
    static const unsigned od_rexw     = 0x00000800 | od_rex_pres;
    static const unsigned od_rexwb    = 0x00000900 | od_rex_pres;
    static const unsigned od_rexwx    = 0x00000a00 | od_rex_pres;
    static const unsigned od_rexwxb   = 0x00000b00 | od_rex_pres;
    static const unsigned od_rexwr    = 0x00000c00 | od_rex_pres;
    static const unsigned od_rexwrb   = 0x00000d00 | od_rex_pres;
    static const unsigned od_rexwrx   = 0x00000e00 | od_rex_pres;
    static const unsigned od_rexwrxb  = 0x00000f00 | od_rex_pres;
    static uint8_t od_rex_byte(unsigned opcode_mods) { return 0x40 | ((opcode_mods & od_rex_mask) >> 8); }

    /** Indicates that the ModR/M byte of the instruction contains a register operand and an r/m operand.  This form is
     *  written as "/r" in the Intel documentation. */
    static const unsigned od_modrm    = 0x00000002;

    /** A 1-byte (CB), 2-byte (CW), 4-byte (CD), 6-byte (CP), 8-byte (CO), or 10-byte (CT) value follows the opcode. This
     *  value is used to specify a code offset and possibly a new value for the code segment register. */
    static const unsigned od_c_mask   = 0x00007000;
    static const unsigned od_cb       = 0x00001000;
    static const unsigned od_cw       = 0x00002000;
    static const unsigned od_cd       = 0x00003000;
    static const unsigned od_cp       = 0x00004000;
    static const unsigned od_co       = 0x00005000;
    static const unsigned od_ct       = 0x00006000;

    /** A 1-byte (IB), 2-byte (IW), 4-byte (ID), or 8-byte (IO) little-endian immediate operand to the instruction follows the
     *  opcode, ModR/M bytes or scale-indexing bytes. The opcode determines if the operand is a signed value. */
    static const unsigned od_i_mask   = 0x00070000;
    static const unsigned od_ib       = 0x00010000;
       static const unsigned od_iw       = 0x00020000;
    static const unsigned od_id       = 0x00030000;
    static const unsigned od_io       = 0x00040000;

    /** A register code, from 0 through 7, added to a byte of the opcode. This form is written as "+rb" in the Intel
     *  documentation. */
    static const unsigned od_r_mask   = 0x00700000;
    static const unsigned od_rb       = 0x00100000;
    static const unsigned od_rw       = 0x00200000;
    static const unsigned od_rd       = 0x00300000;
    static const unsigned od_ro       = 0x00400000;

    /** A number used in floating-point instructions when one of the operands is ST(i) from the FPU register stack. The number
     *  i (which can range from 0 to 7) is added to the opcode byte form a single opcode byte. This form is written as "+i" in
     *  the Intel documentation. */
    static const unsigned od_i        = 0x00000004;


    /** Operand types, from Intel "Instruction Set Reference, A-M" section 3.1.1.2, Vol. 2A 3-3 */
    enum OperandDefn 
        {
        od_none,        /**< Operand is not present as part of the instruction. */
        od_AL,          /**< AL register. */
        od_AX,          /**< AX register. */
        od_EAX,         /**< EAX register. */
        od_RAX,         /**< RAX register. */
        od_DX,          /**< DX register. */
        od_CS,          /**< CS register. */
        od_DS,          /**< DS register. */
        od_ES,          /**< ES register. */
        od_FS,          /**< FS register. */
        od_GS,          /**< GS register. */
        od_SS,          /**< SS register. */
        od_rel8,        /**< A relative address in the range from 128 bytes before the end of the instruction to 127 bytes
                         *   after the end of the instruction. */
        od_rel16,       /**< A relative address in the same code segment as the instruction assembled, with an operand size
                         *   attribute of 16 bits. */
        od_rel32,       /**< A relative address in the same code segment as the instruction assembled, with an operand size
                         *   attribute of 32 bits. */
        od_rel64,       /**< A relative address in the same code segment as the instruction assembled, with an operand size
                         *   attribute of 64 bits. */
        od_ptr16_16,    /**< A far pointer, typically to a code segment different from that of the instruction. The notation
                         *   16:16 indicates that the value of the pointer has two parts. The value to the left of the colon is
                         *   a 16-bit selector or value destined for the code segment register. The value to the right
                         *   corresponds to the offset within the destination segment. The ptr1616 symbol is used when the
                         *   instruction's operand-size attribute is 16 bits. */
        od_ptr16_32,    /**< A far pointer, typically to a code segment different from that of the instruction. The notation
                         *   16:32 indicates that the value of the pointer has two parts. The value to the left of the colon is
                         *   a 16-bit selector or value destined for the code segment register. The value to the right
                         *   corresponds to the offset within the destination segment. The ptr1632 symbol is used when the
                         *   instruction's operand-size attribute is 32 bits. */
        od_ptr16_64,    /**< A far pointer, typically to a code segment different from that of the instruction. The notation
                         *   16:64 indicates that the value of the pointer has two parts. The value to the left of the colon is
                         *   a 16-bit selector or value destined for the code segment register. The value to the right
                         *   corresponds to the offset within the destination segment. The ptr1664 symbol is used when the
                         *   instruction's operand-size attribute is 64 bits. */
        od_r8,          /**< One of the byte general-purpose registers: AL, CL, DL, BL, AH, CH, DH, BH, BPL, SPL, DIL and SIL;
                         *   or one of the byte registers (R8L-R15L) available when using REX.R and 64-bit mode. */
        od_r16,         /**< One of the word general-purpose registers: AX, CX, DX, BX, SP, BP, SI, DI; or one of the word
                         *   registers (R8-R15) available when using REX.R and 64-bit mode. */
        od_r32,         /**< One of the doubleword general-purpose registers: EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI; or one
                         *   of the doubleword registers (R8D-R15D) available when using REX.R and 64-bit mode. */
        od_r64,         /**< One of the quadword general-purpose registers: RAX, RBX, RCX, RDX, RDI, RSI, RBP, RSP, R8-R15.
                         *   These are available when using REX.R and 64-bit mode. */
        od_imm8,        /**< An immediate byte value, a signed number between -128 and +127, inclusive. For instructions in
                         *   which imm8 is combined with a word or doubleword operand, the immediate value is sign-extended to
                         *   form a word or doubleword (the upper byte of the word is filled with the topmost bit of the
                         *   immediate value). */
        od_imm16,       /**< An immediate word value used for instructions whose operand-size attribute is 16 bits. This is a
                         *   number between -32,768 and +32,767 inclusive. */
        od_imm32,       /**< An immediate doubleword value used for instructions whose operand-size attribute is 32 bits. It
                         *  allows the use of a number between -2,147,483,648 and +2,147,483,647 inclusive. */
        od_imm64,       /**< An immediate quadword value used for instructions whose operand-size attribute is 64 bits. The
                         *   value allows the use of a number between -9,223,372,036,854,775,808 and +9,223,372,036,854,775,807
                         *   inclusive. */
        od_r_m8,        /**< A byte operand that is either the contents of a byte general-purpose register (AL, CL, DL, BL, AH,
                         *   CH, DH, BH, BPL, SPL, DIL and SIL) or a byte from memory. Byte registers R8L-R15L are available
                         *   using REX.R in 64-bit mode. This is indicated as "r/m8" in Intel documentation. */
        od_r_m16,       /**< A word general-purpose register or memory operand used for instructions whose operand-size
                         *   attribute is 16-bits. The word general-purpose registers are AX, CX, DX, BX, SP, BP, SI, and DI.
                         *   The contents of memory are found at the address provided by the effective address computation. Word
                         *   registers R8W-R15W are available using REX.R in 64-bit mode. This is indicated as "r/m16" in
                         *   Intel documentation. */
        od_r_m32,       /**< A doubleword general-purpose register or memory operand used for instructions whose operand-size
                         *   attribute is 32-bits. The doubleword general-purpose registers are EAX, ECX, EDX, EBX, ESP, EBP,
                         *   ESI, and EDI. The contents of memory are found at the address provided by the effective address
                         *   computation. Doubleword registers R8D-R15D are available when using REX.R in 64-bit mode. This is
                         *   indicated as "r/m32" in Intel documentation. */
        od_r_m64,       /**< A quadword general-purpose register or memory operand used for instructions whose operand-size
                         *   attribute is 64 bits when using REX.W. Quadword general-purpose registers are RAX, RBX, RCX, RDX,
                         *   RDI, RSI, RBP, RSP, R8-R15; these are available only in 64-bit mode. The contents of memory are
                         *   found at the address provided by the effective address computation. This is indicated as "r/m64"
                         *   in Intel documentation. */
        od_m,           /**< A 16-, 32-, or 64-bit operand in memory. */
        od_m8,          /**< A byte operand in memory, usually expressed as a variable or array name, but pointed to by the
                         *   DS:(E)SI or ES:(E)DI registers. In 64-bit mode, it is pointed to by the RSI or RDI registers. */
        od_m16,         /**< A word operand in memory, usually expressed as a variable or array name, but pointed to by the
                         *   DS:(E)SI or ES:(E)DI registers. This nomenclature is used only with the string instructions. */
        od_m32,         /**< A doubleword operand in memory, usually expressed as a variable or array name, but pointed to by
                         *   the DS:(E)SI or ES:(E)DI registers. This nomenclature is used only with the string instructions. */
        od_m64,         /**< A memory quadword operand in memory. */
        od_m128,        /**< A memory double quadword operand in memory. */
        od_m16_16,      /**< A memory operand containing a far pointer composed of two numbers. In the notation 16:16, the
                         *   number to the left of the colon corresponds to the pointer's segment selector while the number to
                         *   the right corresponds to its offset. */
        od_m16_32,      /**< A memory operand containing a far pointer composed of two numbers. In the notation 16:16, the
                         *   number to the left of the colon corresponds to the pointer's segment selector while the number to
                         *   the right corresponds to its offset. */
        od_m16_64,      /**< A memory operand containing a far pointer composed of two numbers. In the notation 16:16, the
                         *   number to the left of the colon corresponds to the pointer's segment selector while the number to
                         *   the right corresponds to its offset. */
        od_m16a16,      /**< A memory operand consisting of data item pairs whose sizes are indicated on the left and the
                         *   right side of the "a" (normally written "m16&16" in Intel manuals). All memory addressing modes are
                         *   allowed. This operand definition is used by the BOUND instruction to provide an operand containing
                         *   an upper and lower bound for array indices. */
        od_m16a32,      /**< A memory operand consisting of data item pairs whose sizes are indicated on the left and the
                         *   right side of the "a" (normally written "m16&32" in Intel manuals). All memory addressing modes are
                         *   allowed. This operand definition is used by the LIDT and LGDT to provide a word with which to
                         *   load the limit field, and a doubleword with which to load the base field of the corresponding
                         *   GDTR and IDTR registers. */
        od_m32a32,      /**< A memory operand consisting of data item pairs whose sizes are indicated on the left and the
                         *   right side of the "a" (normally written "m32&32" in Intel manuals). All memory addressing modes are
                         *   allowed. This operand definition is used by the BOUND instruction to provide an operand containing
                         *   an upper and lower bound for array indices. */
        od_m16a64,      /**< A memory operand consisting of data item pairs whose sizes are indicated on the left and the
                         *   right side of the "a" (normally written "m64&64" in Intel manuals). All memory addressing modes are
                         *   allowed. This operand definition is used by the LIDT and LGDT in 64-bit mode to provide a word
                         *   with which to load the limit field, and a quadword with which to load the base field of the
                         *   corresponding GDTR and IDTR registers. */
        od_moffs8,      /**< A simple memory variable (memory offset) of type byte used by some variants of the MOV
                         *   instruction. The actual address is given by a simple offset relative to the segment base. No ModR/M
                         *   byte is used in the instruction. This is used by instructions with a 8-bit address size attribute. */
        od_moffs16,      /**< A simple memory variable (memory offset) of type word used by some variants of the MOV
                         *   instruction. The actual address is given by a simple offset relative to the segment base. No ModR/M
                         *   byte is used in the instruction. This is used by instructions with a 16-bit address size attribute. */
        od_moffs32,     /**< A simple memory variable (memory offset) of type doubleword used by some variants of the MOV
                         *   instruction. The actual address is given by a simple offset relative to the segment base. No ModR/M
                         *   byte is used in the instruction. This is used by instructions with a 32-bit address size attribute. */
        od_moffs64,     /**< A simple memory variable (memory offset) of type quadword used by some variants of the MOV
                         *   instruction. The actual address is given by a simple offset relative to the segment base. No ModR/M
                         *   byte is used in the instruction. This is used by instructions with a 64-bit address size attribute. */
        od_sreg,        /**< A segment register. The segment register bit assignments are ES=0, CS=1, SS=2, DS=3, FS=4, and
                         *   GS=5. */
        od_m32fp,       /**< A single-precision floating-point operand in memory used as operands for x87 FPU floating-point
                         *   instructions. */
        od_m64fp,       /**< A double-precision floating-point operand in memory used as operands for x87 FPU floating-point
                         *   instructions. */
        od_m80fp,       /**< A double extended-precision floating-point operand in memory used as operands for x87 FPU
                         *   floating-point instructions. */
        od_st0,         /**< The 0th (top) element of the FPU register stack. */
        od_st1,         /**< The 1st (second-from-top) element of the FPU register stack. */
        od_st2,         /**< The 2nd element of the FPU register stack. */
        od_st3,         /**< The 3rd element of the FPU register stack. */
        od_st4,         /**< The 4th element of the FPU register stack. */
        od_st5,         /**< The 5th element of the FPU register stack. */
        od_st6,         /**< The 6th element of the FPU register stack. */
        od_st7,         /**< The 7th (bottom) element of the FPU register stack. */
        od_sti,         /**< Any element of the FPU register stack. */
        od_mm,          /**< An MMX register. The 64-bit MMX registers are MM0 through MM7. */
        od_mm_m32,      /**< The low-order 32 bits of an MMX register or a 32-bit memory operand. The contents of memory are
                         *   found at the address provided by the effective address computation. This definition is called
                         *   "mm/m32" in Intel documentation. */
        od_mm_m64,      /**< An MMX register or a 64-bit memory operand. The contents of memory are found at the address
                         *   provided by the effective address computation. This definition is called "mm/m64" in Intel
                         *   documentation. */
        od_xmm,         /**< An XMM register. The 128-bit XMM registers are XMM0 through XMM7; XMM8 through XMM15 are
                         *   available using REX.R in 64-bit mode. */
        od_xmm_m16,     /**< See PMOVSXBQ. */
        od_xmm_m32,     /**< An XMM register or a 32-bit memory operand. The 128-bit XMM registers are XMM0 through XMM7; XMM8
                         *   through XMM15 are available using REX.R in 64-bit mode.  The contents of memory are found at the
                         *   address provided by the effective address computation. */
        od_xmm_m64,     /**< An XMM register or a 64-bit memory operand. The 128-bit SIMD floating-point registers are XMM0
                         *   through XMM7; XMM8 through XMM15 are available using REX.R in 64-bit mode.  The contents of memory
                         *   are found at the address provided by the effective address computation. */
        od_xmm_m128,    /**< An XMM register or a 128-bit memory operand. The 128-bit SIMD floating-point registers are XMM0
                         *   through XMM7; XMM8 through XMM15 are available using REX.R in 64-bit mode.  The contents of memory
                         *   are found at the address provided by the effective address computation. */
        /* The following are not documented in section 3.1.1.2 but are used elsewhere in the manual */
        od_XMM0,        /**< See BLENDVPD. */
        od_0,           /**< See ENTER. */
        od_1,           /**< See ENTER. */
        od_m80,         /**< See FBLD. */
        od_dec,         /**< See FBLD. */
        od_m80bcd,      /**< See FBSTP. */
        od_m2byte,      /**< See FLDCW. */
        od_m14_28byte,  /**< See FLDENV. */
        od_m94_108byte, /**< See FRSTOR. */
        od_m512byte,    /**< See FXRSTORE. */
        od_r16_m16,     /**< See LAR. */
        od_r32_m8,      /**< See PINSRB. */
        od_r32_m16,     /**< See LAR. */
        od_r64_m16,     /**< See SLDT. */
        od_CR0,         /**< See MOV. */
        od_CR7,         /**< See MOV. */
        od_CR8,         /**< See MOV. */
        od_CR0CR7,      /**< See MOV. */
        od_DR0DR7,      /**< See MOV. */
        od_reg,         /**< See MOVMSKPD. */
        od_CL,          /**< See SAR. */
    };

    /** These bits define the compatibility of an instruction to 32- and 64-bit modes. */
    static const unsigned COMPAT_LEGACY = 0x01; /**< Definition is compatible with non 64-bit architectures. */
    static const unsigned COMPAT_64     = 0x02; /**< Definition is compatible with 64-bit architectures. */

    /** Returns a ModR/M byte constructed from the three standard fields: mode, register, and register/memory. */
    static uint8_t build_modrm(unsigned mod, unsigned reg, unsigned rm) {
        return ((mod&0x3)<<6) | ((reg&0x7)<<3) | (rm&0x7);
    }

    /** Returns the mode field of a ModR/M byte. */
    static unsigned modrm_mod(uint8_t modrm) { return modrm>>6; }

    /** Returns the register field of a ModR/M byte. */
    static unsigned modrm_reg(uint8_t modrm) { return (modrm>>3) & 0x7; }

    /** Returns the register/memory field of a ModR/M byte. */
    static unsigned modrm_rm(uint8_t modrm) { return modrm & 0x7; }

    /** Returns a SIB byte constructed from the three standard fields: scale, index, and base. */
    static uint8_t build_sib(unsigned ss, unsigned index, unsigned base) {
        return ((ss&0x3)<<6) | ((index&0x7)<<3) | (base&0x7);
    }

    /** Returns the scale field of a SIB byte. */
    static unsigned sib_ss(uint8_t sib) {return sib>>6; }

    /** Returns the index field of a SIB byte. */
    static unsigned sib_index(uint8_t sib) { return (sib>>3) & 0x7; }

    /** Returns the base field of a SIB byte. */
    static unsigned sib_base(uint8_t sib) { return sib & 0x7; }
    
    /** Defines static characteristics of an instruction used by the assembler and disassembler. The @p opcode contains up to
     *  eight bytes, but leading zeros are not emitted during assembly (except when the opcode is zero, in which case a single
     *  zero byte is emitted; see ADD). */
    class InsnDefn {
    public:
        InsnDefn(const std::string &mnemonic, X86InstructionKind kind, unsigned compatibility, uint64_t opcode,
                 unsigned opcode_modifiers, OperandDefn op1=od_none, OperandDefn op2=od_none, OperandDefn op3=od_none,
                 OperandDefn op4=od_none)
            : mnemonic(mnemonic), kind(kind), compatibility(compatibility), opcode(opcode), opcode_modifiers(opcode_modifiers) {
            if (op1) operands.push_back(op1);
            if (op2) operands.push_back(op2);
            if (op3) operands.push_back(op3);
            if (op4) operands.push_back(op4);
        }
        std::string to_str() const;
        void set_location(const std::string &s) {
            location = s;
        }
        std::string mnemonic;
        X86InstructionKind kind;
        unsigned compatibility;
        uint64_t opcode;
        unsigned opcode_modifiers;
        std::vector<OperandDefn> operands;
        std::string location;                           /* location of instruction documentation */
    };

    enum MemoryReferencePattern 
        {
        mrp_unknown,
        mrp_disp,                       /* displacement */
        mrp_index,                      /* register*scale */
        mrp_index_disp,                 /* register*scale + displacement */
        mrp_base,                       /* register */
        mrp_base_disp,                  /* register + displacement */
        mrp_base_index,                 /* register + register*scale */
        mrp_base_index_disp             /* register + register*scale + displacement */
    };

    /** Instruction assembly definitions for a single kind of instruction. */
    typedef std::vector<const InsnDefn*> DictionaryPage;

    /** Instruction assembly definitions for all kinds of instructions. */
    typedef std::map<X86InstructionKind, DictionaryPage> InsnDictionary;

    /** Build the dictionary used by the x86 assemblers. All x86 assemblers share a common dictionary. */
    static void initAssemblyRules();
    static void initAssemblyRules_part1();
    static void initAssemblyRules_part2();
    static void initAssemblyRules_part3();
    static void initAssemblyRules_part4();
    static void initAssemblyRules_part5();
    static void initAssemblyRules_part6();
    static void initAssemblyRules_part7();
    static void initAssemblyRules_part8();
    static void initAssemblyRules_part9();

    /** Adds a definition to the assembly dictionary. All x86 assemblers share a common dictionary. */
    static void define(const InsnDefn *d) {
        defns[d->kind].push_back(d);
    }
    
    /** Returns the string version of the instruction kind sans "x86_" prefix. This is not necessarily the same as the
     *  mnemonic since occassionally multiple kinds will map to a single mnemonic (e.g., RET maps to both x86_ret and
     *  x86_retf). */
    static std::string to_str(X86InstructionKind);

    /** Rewrites the prefix bytes stored in the original @p source to be in the same order (and same repeat counts) as stored
     *  in the target, or p_raw_bytes data member of the instruction. The @p source should contain only prefix bytes from
     *  groups 1 through 4 as listed in section 2.1.1 of the Intel Instruction Set Reference. It should not contain the REX
     *  byte. Any source prefix that does not appear in the original instruction will be placed at the end of the result; any
     *  prefix that appears in the original instruction but not the source will be dropped. */
    SgUnsignedCharList fixup_prefix_bytes(SgAsmX86Instruction *insn, SgUnsignedCharList source);

    /** Low-level method to assemble a single instruction using the specified definition from the assembly dictionary. An
     *  Assembler::Exception is thrown if the instruction is not compatible with the definition. */
    SgUnsignedCharList assemble(SgAsmX86Instruction *insn, const InsnDefn *defn);

    /** Attempts to match an instruction with a definition. An exception is thrown if the instruction and definition do not
     *  match.  If the @p disp or @p imm arguments are non-null pointers then the operands of the instruction are also checked,
     *  and any operand which is an IP-relative displacement or immediate have their values returned through those arguments. */
    void matches(const InsnDefn *defn, SgAsmX86Instruction *insn, int64_t *disp, int64_t *imm) const;

    /** Attempts to match an instruction operand with a definition operand. Returns true if they match, false otherwise. The
     *  @p disp and @p imm pointers are used to return values if the operand is an IP-relative displacement or immediate value. */
    bool matches(OperandDefn, SgAsmExpression*, SgAsmInstruction*, int64_t *disp, int64_t *imm) const;

    /** Determines whether a call/jump target can be represented as a IP-relative displacement of the specified size. */
    static bool matches_rel(SgAsmInstruction*, int64_t val, size_t nbytes);
        
    /** Parses memory refernce expressons and returns the address BASE_REG + (INDEX_REG * SCALE) + DISPLACEMENT, where
     *  BASE_REG and INDEX_REG are optional register reference expressions and SCALE and DISPLACEMENT are optional value
     *  expressions. */
    static MemoryReferencePattern parse_memref(SgAsmInstruction *insn, SgAsmMemoryReferenceExpression *expr,
                                               SgAsmRegisterReferenceExpression **base_reg/*out*/,
                                               SgAsmRegisterReferenceExpression **index_reg/*out*/,
                                               SgAsmValueExpression **scale/*out*/, SgAsmValueExpression **displacement/*out*/);

    /** Builds the ModR/M byte, SIB byte. Also adjusts the REX prefix byte and returns any displacement value. */
    uint8_t build_modrm(const InsnDefn*, SgAsmX86Instruction*, size_t argno,
                        uint8_t *sib, int64_t *displacement, uint8_t *rex) const;

    /** Adjusts the "reg" field of the ModR/M byte and adjusts the REX prefix byte if necessary. */
    void build_modreg(const InsnDefn*, SgAsmX86Instruction*, size_t argno, uint8_t *modrm, uint8_t *rex) const;

    /** Calculates the segment override from the instruction operands rather than obtaining it from the p_segmentOverride data
     *  member. Returns zero if no segment override is necessary. */
    uint8_t segment_override(SgAsmX86Instruction*);

    static InsnDictionary defns;                /**< Instruction assembly definitions organized by X86InstructionKind. */
    bool honor_operand_types;                   /**< If true, operand types rather than values determine assembled form. */
};

} // namespace
} // namespace

#endif
#endif
