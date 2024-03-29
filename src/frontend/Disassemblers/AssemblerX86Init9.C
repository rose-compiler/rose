//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       D O   N O T   E D I T                                                      //
//------------------------------------------------------------------------------------------------------------------//
//                                                                                                                  //
// This file was generated automatically from Intel PDF documentation. Its purpose is to intialize the ROSE x86     //
// assembler with information about how to encode each x86 instruction.                                             //
//                                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include "AssemblerX86.h"

namespace Rose {
namespace BinaryAnalysis {

/* Generated by ./IntelAssemblyBuilder from 
 * x86-InstructionSetReference-AM.pdf 
 * x86-InstructionSetReference-NZ.pdf 
 * ExtraInstructions.txt */
void AssemblerX86::initAssemblyRules_part9() {
    AssemblerX86::InsnDefn *defn = NULL;
    defn = new InsnDefn("vmsave", x86_vmsave,  0x02, 0x0f01db, od_none);
    defn->set_location("ExtraInstructions.txt, page 1");
    define(defn);
    // 0F 01 DC                 STGI               Valid Invalid  VMX Instruction; ModR/M=0334 (0xdc)
    defn = new InsnDefn("stgi",   x86_stgi,    0x02, 0x0f01dc, od_none);
    defn->set_location("ExtraInstructions.txt, page 1");
    define(defn);
    // 0F 01 DD                 CLGI               Valid Invalid  VMX Instruction; ModR/M=0335 (0xdd)
    defn = new InsnDefn("clgi",   x86_clgi,    0x02, 0x0f01dd, od_none);
    defn->set_location("ExtraInstructions.txt, page 1");
    define(defn);
    // 0F 01 DE                 SKINIT             Valid Invalid  VMX Instruction; ModR/M=0336 (0xde)
    defn = new InsnDefn("skinit", x86_skinit,  0x02, 0x0f01de, od_none);
    defn->set_location("ExtraInstructions.txt, page 1");
    define(defn);
    // 0F 01 DF                 INVLPGA            Valid Invalid  VMX Instruction; ModR/M=0337 (0xdf)
    defn = new InsnDefn("invlpga", x86_invlpga, 0x02, 0x0f01df, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);

    //--- page 2 of ExtraInstructions.txt ------------------------------------------------------------------------------
    // 0F 0F 0C                 PI2FW              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pi2fw",  x86_pi2fw,   0x00, 0x0f0f0c, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 0D                 PI2FD              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pi2fd",  x86_pi2fd,   0x00, 0x0f0f0d, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 1C                 PF2IW              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pf2iw",  x86_pf2iw,   0x00, 0x0f0f1c, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 1D                 PF2ID              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pf2id",  x86_pf2id,   0x00, 0x0f0f1d, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 8A                 PFNACC             Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfnacc", x86_pfnacc,  0x00, 0x0f0f8a, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 8E                 PFPNACC            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfpnacc", x86_pfpnacc, 0x00, 0x0f0f8e, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 90                 PFCMPGE            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfcmpge", x86_pfcmpge, 0x00, 0x0f0f90, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 94                 PFMIN              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfmin",  x86_pfmin,   0x00, 0x0f0f94, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 96                 PFRCP              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfrcp",  x86_pfrcp,   0x00, 0x0f0f96, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 97                 PFRSQRT            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfrsqrt", x86_pfrsqrt, 0x00, 0x0f0f97, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 9A                 PFSUB              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfsub",  x86_pfsub,   0x00, 0x0f0f9a, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F 9E                 PFADD              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfadd",  x86_pfadd,   0x00, 0x0f0f9e, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F A0                 PFCMPGT            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfcmpgt", x86_pfcmpgt, 0x00, 0x0f0fa0, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F A4                 PFMAX              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfmax",  x86_pfmax,   0x00, 0x0f0fa4, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F A6                 PFRCPIT1           Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfrcpit1", x86_pfrcpit1, 0x00, 0x0f0fa6, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F A7                 PFRSQIT1           Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfrsqit1", x86_pfrsqit1, 0x00, 0x0f0fa7, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F AA                 PFSUBR             Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfsubr", x86_pfsubr,  0x00, 0x0f0faa, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F AE                 PFACC              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfacc",  x86_pfacc,   0x00, 0x0f0fae, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F B0                 PFCMPEQ            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfcmpeq", x86_pfcmpeq, 0x00, 0x0f0fb0, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F B4                 PFMUL              Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfmul",  x86_pfmul,   0x00, 0x0f0fb4, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F B6                 PFRCPIT2           Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pfrcpit2", x86_pfrcpit2, 0x00, 0x0f0fb6, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F B7                 PMULHRW            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pmulhrw", x86_pmulhrw, 0x00, 0x0f0fb7, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F BB                 PSWAPD             Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pswapd", x86_pswapd,  0x00, 0x0f0fbb, od_none);
    defn->set_location("ExtraInstructions.txt, page 2");
    define(defn);
    // 0F 0F BF                 PAVGUSB            Invalid Invalid  AMD 3DNow!
    defn = new InsnDefn("pavgusb", x86_pavgusb, 0x00, 0x0f0fbf, od_none);
    defn->set_location("ExtraInstructions.txt, page 3");
    define(defn);

    //--- page 3 of ExtraInstructions.txt ------------------------------------------------------------------------------
    // F3 0F BD /r              LZCNT r16, r/m16   Valid   Valid    Alias for REPE BSR
    defn = new InsnDefn("lzcnt",  x86_lzcnt,   0x03, 0xf30fbd, od_modrm, od_r16, od_r_m16);
    defn->set_location("ExtraInstructions.txt, page 3");
    define(defn);
    // F3 0F BD /r              LZCNT r32, r/m32   Valid   Valid    Alias for REPE BSR
    defn = new InsnDefn("lzcnt",  x86_lzcnt,   0x03, 0xf30fbd, od_modrm, od_r32, od_r_m32);
    defn->set_location("ExtraInstructions.txt, page 3");
    define(defn);
    // F3 REX.W + 0F BD         LZCNT r64, r/m64   Valid   N.E.     Alias for REPE BSR
    defn = new InsnDefn("lzcnt",  x86_lzcnt,   0x02, 0xf30fbd, od_rexw, od_r64, od_r_m64);
    defn->set_location("ExtraInstructions.txt, page 4");
    define(defn);

    //--- page 4 of ExtraInstructions.txt ------------------------------------------------------------------------------
    // F2 0F 2B /r              MOVNTSD m128       Valid   Valid    Alias for REPNE MOVNTPS
    defn = new InsnDefn("movntsd", x86_movntsd, 0x03, 0xf20f2b, od_modrm, od_m128);
    defn->set_location("ExtraInstructions.txt, page 4");
    define(defn);
    // F3 0F 2B /r              MOVNTSS m128       Valid   Valid    Alias for REPE MOVNTPS
    defn = new InsnDefn("movntss", x86_movntss, 0x03, 0xf30f2b, od_modrm, od_m128);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);

    //--- page 5 of ExtraInstructions.txt ------------------------------------------------------------------------------
    // 0F 0D /1                 PREFETCHW          Invalid Invalid  Not documented
    defn = new InsnDefn("prefetchw", x86_prefetchw, 0x00, 0x0f0d, od_e1);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F 0D /3                 PREFETCHW          Invalid Invalid  Not documented
    defn = new InsnDefn("prefetchw", x86_prefetchw, 0x00, 0x0f0d, od_e3);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F 0E                    FEMMS              Invalid Invalid  Undocumented by Intel or OIR
    defn = new InsnDefn("femms",  x86_femms,   0x00, 0x0f0e, od_none);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F 18                    PREFETCH           Invalid Invalid  See PREFETCH(0|1|2|NTA) instead
    defn = new InsnDefn("prefetch", x86_prefetch, 0x00, 0x0f18, od_none);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F 37                    GETSEC             Valid   Invalid  Safer mode extensions. See Intel Instruction Set Reference, section 6
    defn = new InsnDefn("getsec", x86_getsec,  0x02, 0x0f37, od_none);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F 73 /4                 PSRAQ r/m64 imm8   Invalid Invalid  Not documented; ModR/M=034x
    defn = new InsnDefn("psraq",  x86_psraq,   0x00, 0x0f73, od_e4|od_ib, od_r_m64, od_imm8);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // F2 0F 78                 INSERTQ imm8 imm8  Invalid Invalid  Undocumented by Intel or OIR
    defn = new InsnDefn("insertq", x86_insertq, 0x00, 0xf20f78, od_none, od_imm8, od_imm8);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 66 0F 79 /0              EXTRQ imm8 imm8    Invalid Invalid  Undocumented by Intel or OIR
    defn = new InsnDefn("extrq",  x86_extrq,   0x00, 0x660f79, od_e0, od_imm8, od_imm8);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 66 90 NOP    Valid   Valid    2-byte form of no-operation
    defn = new InsnDefn("nop",    x86_nop,     0x03, 0x6690, od_none);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F 99                    SETNS r/m8         Invalid Invalid  Set byte condition not signed
    defn = new InsnDefn("setns",  x86_setns,   0x00, 0x0f99, od_none, od_r_m8);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // REX + 0F 99              SETNS r/m8         Invalid N.E.     Set byte condition not signed
    defn = new InsnDefn("setns",  x86_setns,   0x00, 0x0f99, od_rex, od_r_m8);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // 0F B8 /r                 JMPE               Invalid Invalid  Used to enter IA64 mode on Itanium architecture
    defn = new InsnDefn("jmpe",   x86_jmpe,    0x00, 0x0fb8, od_modrm);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // REX.W + 0F BE /r         MOVSX r64, r/m8    Valid   Invalid  Intel docs have wrong REX byte
    defn = new InsnDefn("movsx",  x86_movsx,   0x02, 0x0fbe, od_rexw|od_modrm, od_r64, od_r_m8);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // D6                       SALC               Invalid Invalid  Not documented
    defn = new InsnDefn("salc",   x86_salc,    0x00, 0xd6, od_none);
    defn->set_location("ExtraInstructions.txt, page 5");
    define(defn);
    // F1                       INT1               Valid   Valid    Undocumented by Intel, but by AMD. Valid for 03+
    defn = new InsnDefn("int1",   x86_int1,    0x03, 0xf1, od_none);
    defn->set_location("ExtraInstructions.txt, page 6");
    define(defn);

    //--- page 6 of ExtraInstructions.txt ------------------------------------------------------------------------------
    // 66 REX.W 8B /r           MOV r64, r/m64     Valid   N.E.     0x66 is ignored when REX.W is present
    defn = new InsnDefn("mov",    x86_mov,     0x02, 0x668b, od_rexw|od_modrm, od_r64, od_r_m64);
    defn->set_location("ExtraInstructions.txt, page 6");
    define(defn);
}

} // namespace
} // namespace

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       D O   N O T   E D I T                                                      //
//------------------------------------------------------------------------------------------------------------------//
//                                                                                                                  //
// This file was generated automatically from Intel PDF documentation. Its purpose is to intialize the ROSE x86     //
// assembler with information about how to encode each x86 instruction.                                             //
//                                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
