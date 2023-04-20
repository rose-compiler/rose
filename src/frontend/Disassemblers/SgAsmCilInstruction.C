#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

// Temporary enumeration of all possibilities
static bool isValidCilInstruction()
{
  CilInstructionKind kind = Cil_unknown_instruction;
  switch (kind) {
    case Cil_unknown_instruction:
    case Cil_nop:           // name="nop",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x00",flow="next",type="Primitive"
    case Cil_break:         // name="break",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x01",flow="break",type="Primitive"
    case Cil_ldarg_0:       // name="ldarg.0",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x02",flow="next",type="Macro",constant="0"
    case Cil_ldarg_1:       // name="ldarg.1",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x03",flow="next",type="Macro",constant="1"
    case Cil_ldarg_2:       // name="ldarg.2",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x04",flow="next",type="Macro",constant="2"
    case Cil_ldarg_3:       // name="ldarg.3",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x05",flow="next",type="Macro",constant="3"
    case Cil_ldloc_0:       // name="ldloc.0",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x06",flow="next",type="Macro",constant="0"
    case Cil_ldloc_1:       // name="ldloc.1",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x07",flow="next",type="Macro",constant="1"
    case Cil_ldloc_2:       // name="ldloc.2",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x08",flow="next",type="Macro",constant="2"
    case Cil_ldloc_3:       // name="ldloc.3",input="Pop0",output="Push1",args="InlineNone",o1="0xFF",o2="0x09",flow="next",type="Macro",constant="3"
    case Cil_stloc_0:       // name="stloc.0",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0A",flow="next",type="Macro",constant="0"
    case Cil_stloc_1:       // name="stloc.1",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0B",flow="next",type="Macro",constant="1"
    case Cil_stloc_2:       // name="stloc.2",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0C",flow="next",type="Macro",constant="2"
    case Cil_stloc_3:       // name="stloc.3",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x0D",flow="next",type="Macro",constant="3"
    case Cil_ldarg_s:       // name="ldarg.s",input="Pop0",output="Push1",args="ShortInlineVar",o1="0xFF",o2="0x0E",flow="next",type="Macro"
    case Cil_ldarga_s:      // name="ldarga.s",input="Pop0",output="PushI",args="ShortInlineVar",o1="0xFF",o2="0x0F",flow="next",type="Macro"
    case Cil_starg_s:       // name="starg.s",input="Pop1",output="Push0",args="ShortInlineVar",o1="0xFF",o2="0x10",flow="next",type="Macro"
    case Cil_ldloc_s:       // name="ldloc.s",input="Pop0",output="Push1",args="ShortInlineVar",o1="0xFF",o2="0x11",flow="next",type="Macro"
    case Cil_ldloca_s:      // name="ldloca.s",input="Pop0",output="PushI",args="ShortInlineVar",o1="0xFF",o2="0x12",flow="next",type="Macro"
    case Cil_stloc_s:       // name="stloc.s",input="Pop1",output="Push0",args="ShortInlineVar",o1="0xFF",o2="0x13",flow="next",type="Macro"
    case Cil_ldnull:        // name="ldnull",input="Pop0",output="PushRef",args="InlineNone",o1="0xFF",o2="0x14",flow="next",type="Primitive"
    case Cil_ldc_i4_m1:     // name="ldc.i4.m1",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x15",flow="next",type="Macro",constant="-1"
    case Cil_ldc_i4_0:      // name="ldc.i4.0",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x16",flow="next",type="Macro",constant="0"
    case Cil_ldc_i4_1:      // name="ldc.i4.1",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x17",flow="next",type="Macro",constant="1"
    case Cil_ldc_i4_2:      // name="ldc.i4.2",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x18",flow="next",type="Macro",constant="2"
    case Cil_ldc_i4_3:      // name="ldc.i4.3",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x19",flow="next",type="Macro",constant="3"
    case Cil_ldc_i4_4:      // name="ldc.i4.4",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1A",flow="next",type="Macro",constant="4"
    case Cil_ldc_i4_5:      // name="ldc.i4.5",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1B",flow="next",type="Macro",constant="5"
    case Cil_ldc_i4_6:      // name="ldc.i4.6",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1C",flow="next",type="Macro",constant="6"
    case Cil_ldc_i4_7:      // name="ldc.i4.7",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1D",flow="next",type="Macro",constant="7"
    case Cil_ldc_i4_8:      // name="ldc.i4.8",input="Pop0",output="PushI",args="InlineNone",o1="0xFF",o2="0x1E",flow="next",type="Macro",constant="8"
    case Cil_ldc_i4_s:      // name="ldc.i4.s",input="Pop0",output="PushI",args="ShortInlineI",o1="0xFF",o2="0x1F",flow="next",type="Macro"
    case Cil_ldc_i4:        // name="ldc.i4",input="Pop0",output="PushI",args="InlineI",o1="0xFF",o2="0x20",flow="next",type="Primitive"
    case Cil_ldc_i8:        // name="ldc.i8",input="Pop0",output="PushI8",args="InlineI8",o1="0xFF",o2="0x21",flow="next",type="Primitive"
    case Cil_ldc_r4:        // name="ldc.r4",input="Pop0",output="PushR4",args="ShortInlineR",o1="0xFF",o2="0x22",flow="next",type="Primitive"
    case Cil_ldc_r8:        // name="ldc.r8",input="Pop0",output="PushR8",args="InlineR",o1="0xFF",o2="0x23",flow="next",type="Primitive"
    case Cil_unused99:      // name="unused99",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x24",flow="next"
    case Cil_dup:           // name="dup",input="Pop1",output="Push1+Push1",args="InlineNone",o1="0xFF",o2="0x25",flow="next",type="Primitive"
    case Cil_pop:           // name="pop",input="Pop1",output="Push0",args="InlineNone",o1="0xFF",o2="0x26",flow="next",type="Primitive"
    case Cil_jmp:           // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call",type="Primitive"
    case Cil_call:          // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call",type="Primitive"
    case Cil_calli:         // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call",type="Primitive"
    case Cil_ret:           // name="ret",input="VarPop",output="Push0",args="InlineNone",o1="0xFF",o2="0x2A",flow="return",type="Primitive"
    case Cil_br_s:          // name="br.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2B",flow="branch",type="Macro"
    case Cil_brfalse_s:     // name="brfalse.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2C",flow="cond-branch",type="Macro"
    case Cil_brtrue_s:      // name="brtrue.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2D",flow="cond-branch",type="Macro"
    case Cil_beq_s:         // name="beq.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2E",flow="cond-branch",type="Macro"
    case Cil_bge_s:         // name="bge.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2F",flow="cond-branch",type="Macro"
    case Cil_bgt_s:         // name="bgt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x30",flow="cond-branch",type="Macro"
    case Cil_ble_s:         // name="ble.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x31",flow="cond-branch",type="Macro"
    case Cil_blt_s:         // name="blt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x32",flow="cond-branch",type="Macro"
    case Cil_bne_un_s:      // name="bne.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x33",flow="cond-branch",type="Macro"
    case Cil_bge_un_s:      // name="bge.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x34",flow="cond-branch",type="Macro"
    case Cil_bgt_un_s:      // name="bgt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x35",flow="cond-branch",type="Macro"
    case Cil_ble_un_s:      // name="ble.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x36",flow="cond-branch",type="Macro"
    case Cil_blt_un_s:      // name="blt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x37",flow="cond-branch",type="Macro"
    case Cil_br:            // name="br",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x38",flow="branch",type="Primitive"
    case Cil_brfalse:       // name="brfalse",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x39",flow="cond-branch",type="Primitive"
    case Cil_brtrue:        // name="brtrue",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3A",flow="cond-branch",type="Primitive"
    case Cil_beq:           // name="beq",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3B",flow="cond-branch",type="Macro"
    case Cil_bge:           // name="bge",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3C",flow="cond-branch",type="Macro"
    case Cil_bgt:           // name="bgt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3D",flow="cond-branch",type="Macro"
    case Cil_ble:           // name="ble",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3E",flow="cond-branch",type="Macro"
    case Cil_blt:           // name="blt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3F",flow="cond-branch",type="Macro"
    case Cil_bne_un:        // name="bne.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x40",flow="cond-branch",type="Macro"
    case Cil_bge_un:        // name="bge.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x41",flow="cond-branch",type="Macro"
    case Cil_bgt_un:        // name="bgt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x42",flow="cond-branch",type="Macro"
    case Cil_ble_un:        // name="ble.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x43",flow="cond-branch",type="Macro"
    case Cil_blt_un:        // name="blt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x44",flow="cond-branch",type="Macro"
    case Cil_switch:        // name="switch",input="PopI",output="Push0",args="InlineSwitch",o1="0xFF",o2="0x45",flow="cond-branch",type="Primitive"
    case Cil_ldind_i1:      // name="ldind.i1",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x46",flow="next",type="Primitive"
    case Cil_ldind_u1:      // name="ldind.u1",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x47",flow="next",type="Primitive"
    case Cil_ldind_i2:      // name="ldind.i2",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x48",flow="next",type="Primitive"
    case Cil_ldind_u2:      // name="ldind.u2",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x49",flow="next",type="Primitive"
    case Cil_ldind_i4:      // name="ldind.i4",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x4A",flow="next",type="Primitive"
    case Cil_ldind_u4:      // name="ldind.u4",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x4B",flow="next",type="Primitive"
    case Cil_ldind_i8:      // name="ldind.i8",input="PopI",output="PushI8",args="InlineNone",o1="0xFF",o2="0x4C",flow="next",type="Primitive"
    // DQ (11/6/2021): Added this enoum missed in the automated mechanism done by Craig.
    // Indirect load value of type unsigned int64 as int64 on the stack (alias for ldind.i8). 
    case Cil_ldind_u8:      // name="ldind.u8",input="PopI",output="PushI8",args="InlineNone",o1="0xFF",o2="0x4C",flow="next",type="Primitive"
    case Cil_ldind_i:       // name="ldind.i",input="PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x4D",flow="next",type="Primitive"
    case Cil_ldind_r4:      // name="ldind.r4",input="PopI",output="PushR4",args="InlineNone",o1="0xFF",o2="0x4E",flow="next",type="Primitive"
    case Cil_ldind_r8:      // name="ldind.r8",input="PopI",output="PushR8",args="InlineNone",o1="0xFF",o2="0x4F",flow="next",type="Primitive"
    case Cil_ldind_ref:     // name="ldind.ref",input="PopI",output="PushRef",args="InlineNone",o1="0xFF",o2="0x50",flow="next",type="Primitive"
    case Cil_stind_ref:     // name="stind.ref",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x51",flow="next",type="Primitive"
    case Cil_stind_i1:      // name="stind.i1",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x52",flow="next",type="Primitive"
    case Cil_stind_i2:      // name="stind.i2",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x53",flow="next",type="Primitive"
    case Cil_stind_i4:      // name="stind.i4",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x54",flow="next",type="Primitive"
    case Cil_stind_i8:      // name="stind.i8",input="PopI+PopI8",output="Push0",args="InlineNone",o1="0xFF",o2="0x55",flow="next",type="Primitive"
    case Cil_stind_r4:      // name="stind.r4",input="PopI+PopR4",output="Push0",args="InlineNone",o1="0xFF",o2="0x56",flow="next",type="Primitive"
    case Cil_stind_r8:      // name="stind.r8",input="PopI+PopR8",output="Push0",args="InlineNone",o1="0xFF",o2="0x57",flow="next",type="Primitive"
    case Cil_add:           // name="add",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x58",flow="next",type="Primitive"
    case Cil_sub:           // name="sub",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x59",flow="next",type="Primitive"
    case Cil_mul:           // name="mul",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5A",flow="next",type="Primitive"
    case Cil_div:           // name="div",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5B",flow="next",type="Primitive"
    case Cil_div_un:        // name="div.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5C",flow="next",type="Primitive"
    case Cil_rem:           // name="rem",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5D",flow="next",type="Primitive"
    case Cil_rem_un:        // name="rem.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5E",flow="next",type="Primitive"
    case Cil_and:           // name="and",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x5F",flow="next",type="Primitive"
    case Cil_or:            // name="or",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x60",flow="next",type="Primitive"
    case Cil_xor:           // name="xor",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x61",flow="next",type="Primitive"
    case Cil_shl:           // name="shl",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x62",flow="next",type="Primitive"
    case Cil_shr:           // name="shr",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x63",flow="next",type="Primitive"
    case Cil_shr_un:        // name="shr.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x64",flow="next",type="Primitive"
    case Cil_neg:           // name="neg",input="Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x65",flow="next",type="Primitive"
    case Cil_not:           // name="not",input="Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0x66",flow="next",type="Primitive"
    case Cil_conv_i1:       // name="conv.i1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x67",flow="next",type="Primitive"
    case Cil_conv_i2:       // name="conv.i2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x68",flow="next",type="Primitive"
    case Cil_conv_i4:       // name="conv.i4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x69",flow="next",type="Primitive"
    case Cil_conv_i8:       // name="conv.i8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x6A",flow="next",type="Primitive"
    case Cil_conv_r4:       // name="conv.r4",input="Pop1",output="PushR4",args="InlineNone",o1="0xFF",o2="0x6B",flow="next",type="Primitive"
    case Cil_conv_r8:       // name="conv.r8",input="Pop1",output="PushR8",args="InlineNone",o1="0xFF",o2="0x6C",flow="next",type="Primitive"
    case Cil_conv_u4:       // name="conv.u4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x6D",flow="next",type="Primitive"
    case Cil_conv_u8:       // name="conv.u8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x6E",flow="next",type="Primitive"
    case Cil_callvirt:      // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call",type="Objmodel"
    case Cil_cpobj:         // name="cpobj",input="PopI+PopI",output="Push0",args="InlineType",o1="0xFF",o2="0x70",flow="next",type="Objmodel"
    case Cil_ldobj:         // name="ldobj",input="PopI",output="Push1",args="InlineType",o1="0xFF",o2="0x71",flow="next",type="Objmodel"
    case Cil_ldstr:         // name="ldstr",input="Pop0",output="PushRef",args="InlineString",o1="0xFF",o2="0x72",flow="next",type="Objmodel"
    case Cil_newobj:        // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call",type="Objmodel"
    case Cil_castclass:     // name="castclass",input="PopRef",output="PushRef",args="InlineType",o1="0xFF",o2="0x74",flow="next",type="Objmodel"
    case Cil_isinst:        // name="isinst",input="PopRef",output="PushI",args="InlineType",o1="0xFF",o2="0x75",flow="next",type="Objmodel"
    case Cil_conv_r_un:     // name="conv.r.un",input="Pop1",output="PushR8",args="InlineNone",o1="0xFF",o2="0x76",flow="next",type="Primitive"
    case Cil_unused58:      // name="unused58",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x77",flow="next"
    case Cil_unused1:       // name="unused1",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x78",flow="next"
    case Cil_unbox:         // name="unbox",input="PopRef",output="PushI",args="InlineType",o1="0xFF",o2="0x79",flow="next",type="Primitive"
    case Cil_throw:         // name="throw",input="PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0x7A",flow="throw",type="Objmodel"
    case Cil_ldfld:         // name="ldfld",input="PopRef",output="Push1",args="InlineField",o1="0xFF",o2="0x7B",flow="next",type="Objmodel"
    case Cil_ldflda:        // name="ldflda",input="PopRef",output="PushI",args="InlineField",o1="0xFF",o2="0x7C",flow="next",type="Objmodel"
    case Cil_stfld:         // name="stfld",input="PopRef+Pop1",output="Push0",args="InlineField",o1="0xFF",o2="0x7D",flow="next",type="Objmodel"
    case Cil_ldsfld:        // name="ldsfld",input="Pop0",output="Push1",args="InlineField",o1="0xFF",o2="0x7E",flow="next",type="Objmodel"
    case Cil_ldsflda:       // name="ldsflda",input="Pop0",output="PushI",args="InlineField",o1="0xFF",o2="0x7F",flow="next",type="Objmodel"
    case Cil_stsfld:        // name="stsfld",input="Pop1",output="Push0",args="InlineField",o1="0xFF",o2="0x80",flow="next",type="Objmodel"
    case Cil_stobj:         // name="stobj",input="PopI+Pop1",output="Push0",args="InlineType",o1="0xFF",o2="0x81",flow="next",type="Primitive"
    case Cil_conv_ovf_i1_un:// name="conv.ovf.i1.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x82",flow="next",type="Primitive"
    case Cil_conv_ovf_i2_un:// name="conv.ovf.i2.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x83",flow="next",type="Primitive"
    case Cil_conv_ovf_i4_un:// name="conv.ovf.i4.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x84",flow="next",type="Primitive"
    case Cil_conv_ovf_i8_un:// name="conv.ovf.i8.un",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x85",flow="next",type="Primitive"
    case Cil_conv_ovf_u1_un:// name="conv.ovf.u1.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x86",flow="next",type="Primitive"
    case Cil_conv_ovf_u2_un:// name="conv.ovf.u2.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x87",flow="next",type="Primitive"
    case Cil_conv_ovf_u4_un:// name="conv.ovf.u4.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x88",flow="next",type="Primitive"
    case Cil_conv_ovf_u8_un:// name="conv.ovf.u8.un",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0x89",flow="next",type="Primitive"
    case Cil_conv_ovf_i_un: // name="conv.ovf.i.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x8A",flow="next",type="Primitive"
    case Cil_conv_ovf_u_un: // name="conv.ovf.u.un",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0x8B",flow="next",type="Primitive"
    case Cil_box:           // name="box",input="Pop1",output="PushRef",args="InlineType",o1="0xFF",o2="0x8C",flow="next",type="Primitive"
    case Cil_newarr:        // name="newarr",input="PopI",output="PushRef",args="InlineType",o1="0xFF",o2="0x8D",flow="next",type="Objmodel"
    case Cil_ldlen:         // name="ldlen",input="PopRef",output="PushI",args="InlineNone",o1="0xFF",o2="0x8E",flow="next",type="Objmodel"
    case Cil_ldelema:       // name="ldelema",input="PopRef+PopI",output="PushI",args="InlineType",o1="0xFF",o2="0x8F",flow="next",type="Objmodel"
    case Cil_ldelem_i1:     // name="ldelem.i1",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x90",flow="next",type="Objmodel"
    case Cil_ldelem_u1:     // name="ldelem.u1",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x91",flow="next",type="Objmodel"
    case Cil_ldelem_i2:     // name="ldelem.i2",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x92",flow="next",type="Objmodel"
    case Cil_ldelem_u2:     // name="ldelem.u2",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x93",flow="next",type="Objmodel"
    case Cil_ldelem_i4:     // name="ldelem.i4",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x94",flow="next",type="Objmodel"
    case Cil_ldelem_u4:     // name="ldelem.u4",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x95",flow="next",type="Objmodel"
    case Cil_ldelem_i8:     // name="ldelem.i8",input="PopRef+PopI",output="PushI8",args="InlineNone",o1="0xFF",o2="0x96",flow="next",type="Objmodel"
    case Cil_ldelem_i:      // name="ldelem.i",input="PopRef+PopI",output="PushI",args="InlineNone",o1="0xFF",o2="0x97",flow="next",type="Objmodel"
    case Cil_ldelem_r4:     // name="ldelem.r4",input="PopRef+PopI",output="PushR4",args="InlineNone",o1="0xFF",o2="0x98",flow="next",type="Objmodel"
    case Cil_ldelem_r8:     // name="ldelem.r8",input="PopRef+PopI",output="PushR8",args="InlineNone",o1="0xFF",o2="0x99",flow="next",type="Objmodel"
    case Cil_ldelem_ref:    // name="ldelem.ref",input="PopRef+PopI",output="PushRef",args="InlineNone",o1="0xFF",o2="0x9A",flow="next",type="Objmodel"
    case Cil_stelem_i:      // name="stelem.i",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9B",flow="next",type="Objmodel"
    case Cil_stelem_i1:     // name="stelem.i1",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9C",flow="next",type="Objmodel"
    case Cil_stelem_i2:     // name="stelem.i2",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9D",flow="next",type="Objmodel"
    case Cil_stelem_i4:     // name="stelem.i4",input="PopRef+PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0x9E",flow="next",type="Objmodel"
    case Cil_stelem_i8:     // name="stelem.i8",input="PopRef+PopI+PopI8",output="Push0",args="InlineNone",o1="0xFF",o2="0x9F",flow="next",type="Objmodel"
    case Cil_stelem_r4:     // name="stelem.r4",input="PopRef+PopI+PopR4",output="Push0",args="InlineNone",o1="0xFF",o2="0xA0",flow="next",type="Objmodel"
    case Cil_stelem_r8:     // name="stelem.r8",input="PopRef+PopI+PopR8",output="Push0",args="InlineNone",o1="0xFF",o2="0xA1",flow="next",type="Objmodel"
    case Cil_stelem_ref:    // name="stelem.ref",input="PopRef+PopI+PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0xA2",flow="next",type="Objmodel"
    case Cil_ldelem:        // name="ldelem",input="PopRef+PopI",output="Push1",args="InlineType",o1="0xFF",o2="0xA3",flow="next",type="Objmodel"
    case Cil_stelem:        // name="stelem",input="PopRef+PopI+Pop1",output="Push0",args="InlineType",o1="0xFF",o2="0xA4",flow="next",type="Objmodel"
    case Cil_unbox_any:     // name="unbox.any",input="PopRef",output="Push1",args="InlineType",o1="0xFF",o2="0xA5",flow="next",type="Objmodel"
    case Cil_unused5:       // name="unused5",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA6",flow="next"
    case Cil_unused6:       // name="unused6",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA7",flow="next"
    case Cil_unused7:       // name="unused7",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA8",flow="next"
    case Cil_unused8:       // name="unused8",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xA9",flow="next"
    case Cil_unused9:       // name="unused9",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAA",flow="next"
    case Cil_unused10:      // name="unused10",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAB",flow="next"
    case Cil_unused11:      // name="unused11",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAC",flow="next"
    case Cil_unused12:      // name="unused12",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAD",flow="next"
    case Cil_unused13:      // name="unused13",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAE",flow="next"
    case Cil_unused14:      // name="unused14",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xAF",flow="next"
    case Cil_unused15:      // name="unused15",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xB0",flow="next"
    case Cil_unused16:      // name="unused16",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xB1",flow="next"
    case Cil_unused17:      // name="unused17",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xB2",flow="next"
    case Cil_conv_ovf_i1:   // name="conv.ovf.i1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB3",flow="next",type="Primitive"
    case Cil_conv_ovf_u1:   // name="conv.ovf.u1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB4",flow="next",type="Primitive"
    case Cil_conv_ovf_i2:   // name="conv.ovf.i2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB5",flow="next",type="Primitive"
    case Cil_conv_ovf_u2:   // name="conv.ovf.u2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB6",flow="next",type="Primitive"
    case Cil_conv_ovf_i4:   // name="conv.ovf.i4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB7",flow="next",type="Primitive"
    case Cil_conv_ovf_u4:   // name="conv.ovf.u4",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xB8",flow="next",type="Primitive"
    case Cil_conv_ovf_i8:   // name="conv.ovf.i8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0xB9",flow="next",type="Primitive"
    case Cil_conv_ovf_u8:   // name="conv.ovf.u8",input="Pop1",output="PushI8",args="InlineNone",o1="0xFF",o2="0xBA",flow="next",type="Primitive"
    case Cil_unused50:      // name="unused50",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBB",flow="next"
    case Cil_unused18:      // name="unused18",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBC",flow="next"
    case Cil_unused19:      // name="unused19",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBD",flow="next"
    case Cil_unused20:      // name="unused20",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBE",flow="next"
    case Cil_unused21:      // name="unused21",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xBF",flow="next"
    case Cil_unused22:      // name="unused22",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC0",flow="next"
    case Cil_unused23:      // name="unused23",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC1",flow="next"
    case Cil_refanyval:     // name="refanyval",input="Pop1",output="PushI",args="InlineType",o1="0xFF",o2="0xC2",flow="next",type="Primitive"
    case Cil_ckfinite:      // name="ckfinite",input="Pop1",output="PushR8",args="InlineNone",o1="0xFF",o2="0xC3",flow="next",type="Primitive"
    case Cil_unused24:      // name="unused24",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC4",flow="next"
    case Cil_unused25:      // name="unused25",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC5",flow="next"
    case Cil_mkrefany:      // name="mkrefany",input="PopI",output="Push1",args="InlineType",o1="0xFF",o2="0xC6",flow="next",type="Primitive"
    case Cil_unused59:      // name="unused59",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC7",flow="next"
    case Cil_unused60:      // name="unused60",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC8",flow="next"
    case Cil_unused61:      // name="unused61",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xC9",flow="next"
    case Cil_unused62:      // name="unused62",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCA",flow="next"
    case Cil_unused63:      // name="unused63",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCB",flow="next"
    case Cil_unused64:      // name="unused64",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCC",flow="next"
    case Cil_unused65:      // name="unused65",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCD",flow="next"
    case Cil_unused66:      // name="unused66",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCE",flow="next"
    case Cil_unused67:      // name="unused67",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xCF",flow="next"
    case Cil_ldtoken:       // name="ldtoken",input="Pop0",output="PushI",args="InlineTok",o1="0xFF",o2="0xD0",flow="next",type="Primitive"
    case Cil_conv_u2:       // name="conv.u2",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD1",flow="next",type="Primitive"
    case Cil_conv_u1:       // name="conv.u1",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD2",flow="next",type="Primitive"
    case Cil_conv_i:        // name="conv.i",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD3",flow="next",type="Primitive"
    case Cil_conv_ovf_i:    // name="conv.ovf.i",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD4",flow="next",type="Primitive"
    case Cil_conv_ovf_u:    // name="conv.ovf.u",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xD5",flow="next",type="Primitive"
    case Cil_add_ovf:       // name="add.ovf",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD6",flow="next",type="Primitive"
    case Cil_add_ovf_un:    // name="add.ovf.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD7",flow="next",type="Primitive"
    case Cil_mul_ovf:       // name="mul.ovf",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD8",flow="next",type="Primitive"
    case Cil_mul_ovf_un:    // name="mul.ovf.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xD9",flow="next",type="Primitive"
    case Cil_sub_ovf:       // name="sub.ovf",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xDA",flow="next",type="Primitive"
    case Cil_sub_ovf_un:    // name="sub.ovf.un",input="Pop1+Pop1",output="Push1",args="InlineNone",o1="0xFF",o2="0xDB",flow="next",type="Primitive"
    case Cil_endfinally:    // name="endfinally",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xDC",flow="return",type="Primitive"
    case Cil_leave:         // name="leave",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0xDD",flow="branch",type="Primitive"
    case Cil_leave_s:       // name="leave.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0xDE",flow="branch",type="Primitive"
    case Cil_stind_i:       // name="stind.i",input="PopI+PopI",output="Push0",args="InlineNone",o1="0xFF",o2="0xDF",flow="next",type="Primitive"
    case Cil_conv_u:        // name="conv.u",input="Pop1",output="PushI",args="InlineNone",o1="0xFF",o2="0xE0",flow="next",type="Primitive"
    case Cil_unused26:      // name="unused26",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE1",flow="next"
    case Cil_unused27:      // name="unused27",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE2",flow="next"
    case Cil_unused28:      // name="unused28",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE3",flow="next"
    case Cil_unused29:      // name="unused29",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE4",flow="next"
    case Cil_unused30:      // name="unused30",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE5",flow="next"
    case Cil_unused31:      // name="unused31",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE6",flow="next"
    case Cil_unused32:      // name="unused32",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE7",flow="next"
    case Cil_unused33:      // name="unused33",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE8",flow="next"
    case Cil_unused34:      // name="unused34",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xE9",flow="next"
    case Cil_unused35:      // name="unused35",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEA",flow="next"
    case Cil_unused36:      // name="unused36",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEB",flow="next"
    case Cil_unused37:      // name="unused37",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEC",flow="next"
    case Cil_unused38:      // name="unused38",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xED",flow="next"
    case Cil_unused39:      // name="unused39",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEE",flow="next"
    case Cil_unused40:      // name="unused40",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xEF",flow="next"
    case Cil_unused41:      // name="unused41",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF0",flow="next"
    case Cil_unused42:      // name="unused42",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF1",flow="next"
    case Cil_unused43:      // name="unused43",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF2",flow="next"
    case Cil_unused44:      // name="unused44",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF3",flow="next"
    case Cil_unused45:      // name="unused45",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF4",flow="next"
    case Cil_unused46:      // name="unused46",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF5",flow="next"
    case Cil_unused47:      // name="unused47",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF6",flow="next"
    case Cil_unused48:      // name="unused48",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF7",flow="next"
    case Cil_prefix7:       // name="prefix7",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF8",flow="meta",type="Nternal"
    case Cil_prefix6:       // name="prefix6",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xF9",flow="meta",type="Nternal"
    case Cil_prefix5:       // name="prefix5",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFA",flow="meta",type="Nternal"
    case Cil_prefix4:       // name="prefix4",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFB",flow="meta",type="Nternal"
    case Cil_prefix3:       // name="prefix3",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFC",flow="meta",type="Nternal"
    case Cil_prefix2:       // name="prefix2",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFD",flow="meta",type="Nternal"
    case Cil_prefix1:       // name="prefix1",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFE",flow="meta",type="Nternal"
    case Cil_prefixref:     // name="prefixref",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xFF",flow="meta",type="Nternal"
    case Cil_arglist:       // name="arglist",input="Pop0",output="PushI",args="InlineNone",o1="0xFE",o2="0x00",flow="next",type="Primitive"
    case Cil_ceq:           // name="ceq",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x01",flow="next",type="Primitive"
    case Cil_cgt:           // name="cgt",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x02",flow="next",type="Primitive"
    case Cil_cgt_un:        // name="cgt.un",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x03",flow="next",type="Primitive"
    case Cil_clt:           // name="clt",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x04",flow="next",type="Primitive"
    case Cil_clt_un:        // name="clt.un",input="Pop1+Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x05",flow="next",type="Primitive"
    case Cil_ldftn:         // name="ldftn",input="Pop0",output="PushI",args="InlineMethod",o1="0xFE",o2="0x06",flow="next",type="Primitive"
    case Cil_ldvirtftn:     // name="ldvirtftn",input="PopRef",output="PushI",args="InlineMethod",o1="0xFE",o2="0x07",flow="next",type="Primitive"
    case Cil_unused56:      // name="unused56",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x08",flow="next"
    case Cil_ldarg:         // name="ldarg",input="Pop0",output="Push1",args="InlineVar",o1="0xFE",o2="0x09",flow="next",type="Primitive"
    case Cil_ldarga:        // name="ldarga",input="Pop0",output="PushI",args="InlineVar",o1="0xFE",o2="0x0A",flow="next",type="Primitive"
    case Cil_starg:         // name="starg",input="Pop1",output="Push0",args="InlineVar",o1="0xFE",o2="0x0B",flow="next",type="Primitive"
    case Cil_ldloc:         // name="ldloc",input="Pop0",output="Push1",args="InlineVar",o1="0xFE",o2="0x0C",flow="next",type="Primitive"
    case Cil_ldloca:        // name="ldloca",input="Pop0",output="PushI",args="InlineVar",o1="0xFE",o2="0x0D",flow="next",type="Primitive"
    case Cil_stloc:         // name="stloc",input="Pop1",output="Push0",args="InlineVar",o1="0xFE",o2="0x0E",flow="next",type="Primitive"
    case Cil_localloc:      // name="localloc",input="PopI",output="PushI",args="InlineNone",o1="0xFE",o2="0x0F",flow="next",type="Primitive"
    case Cil_unused57:      // name="unused57",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x10",flow="next"
    case Cil_endfilter:     // name="endfilter",input="PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x11",flow="return",type="Primitive"
    case Cil_unaligned_:    // name="unaligned.",input="Pop0",output="Push0",args="ShortInlineI",o1="0xFE",o2="0x12",flow="meta",type="Prefix"
    case Cil_volatile_:     // name="volatile.",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x13",flow="meta",type="Prefix"
    case Cil_tail_:         // name="tail.",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x14",flow="meta",type="Prefix"
    case Cil_initobj:       // name="initobj",input="PopI",output="Push0",args="InlineType",o1="0xFE",o2="0x15",flow="next",type="Objmodel"
    case Cil_constrained_:  // name="constrained.",input="Pop0",output="Push0",args="InlineType",o1="0xFE",o2="0x16",flow="meta",type="Prefix"
    case Cil_cpblk:         // name="cpblk",input="PopI+PopI+PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x17",flow="next",type="Primitive"
    case Cil_initblk:       // name="initblk",input="PopI+PopI+PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x18",flow="next",type="Primitive"
    case Cil_no_:           // name="no.",input="Pop0",output="Push0",args="ShortInlineI",o1="0xFE",o2="0x19",flow="next"
    case Cil_rethrow:       // name="rethrow",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1A",flow="throw",type="Objmodel"
    case Cil_unused:        // name="unused",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1B",flow="next"
    case Cil_sizeof:        // name="sizeof",input="Pop0",output="PushI",args="InlineType",o1="0xFE",o2="0x1C",flow="next",type="Primitive"
    case Cil_refanytype:    // name="refanytype",input="Pop1",output="PushI",args="InlineNone",o1="0xFE",o2="0x1D",flow="next",type="Primitive"
    case Cil_readonly_:     // name="readonly.",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1E",flow="meta"
    case Cil_unused53:      // name="unused53",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1F",flow="next"
    case Cil_unused54:      // name="unused54",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x20",flow="next"
    case Cil_unused55:      // name="unused55",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x21",flow="next"
    case Cil_unused70:      // name="unused70",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x22",flow="next"
    case Cil_illegal:       // name="illegal",input="Pop0",output="Push0",args="InlineNone",o1="0x00",o2="0x00",flow="meta"
    case Cil_endmac:        // name="endmac",input="Pop0",output="Push0",args="InlineNone",o1="0x00",o2="0x00",flow="meta"
    case Cil_mono_icall:    // name="mono_icall",input="VarPop",output="VarPush",args="InlineI",o1="0xF0",o2="0x00",flow="next"
    case Cil_mono_objaddr:  // name="mono_objaddr",input="Pop1",output="PushI",args="InlineNone",o1="0xF0",o2="0x01",flow="next"
    case Cil_mono_ldptr:    // name="mono_ldptr",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x02",flow="next"
    case Cil_mono_vtaddr:   // name="mono_vtaddr",input="Pop1",output="PushI",args="InlineNone",o1="0xF0",o2="0x03",flow="next"
    case Cil_mono_newobj:   // name="mono_newobj",input="Pop0",output="PushRef",args="InlineType",o1="0xF0",o2="0x04",flow="next"
    case Cil_mono_retobj:   // name="mono_retobj",input="PopI",output="Push0",args="InlineType",o1="0xF0",o2="0x05",flow="return"
    case Cil_mono_ldnativeobj:// name="mono_ldnativeobj",input="PopI",output="Push1",args="InlineType",o1="0xF0",o2="0x06",flow="return"
    case Cil_mono_cisinst:  // name="mono_cisinst",input="PopRef",output="Push1",args="InlineType",o1="0xF0",o2="0x07",flow="next"
    case Cil_mono_ccastclass:// name="mono_ccastclass",input="PopRef",output="Push1",args="InlineType",o1="0xF0",o2="0x08",flow="next"
    case Cil_mono_save_lmf: // name="mono_save_lmf",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x09",flow="next"
    case Cil_mono_restore_lmf:// name="mono_restore_lmf",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x0A",flow="next"
    case Cil_mono_classconst:// name="mono_classconst",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x0B",flow="next"
    case Cil_mono_not_taken:// name="mono_not_taken",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x0C",flow="next"
    case Cil_mono_tls:      // name="mono_tls",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x0D",flow="next"
    case Cil_mono_icall_addr:// name="mono_icall_addr",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x0E",flow="next"
    case Cil_mono_dyn_call: // name="mono_dyn_call",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x0F",flow="next"
    case Cil_mono_memory_barrier:// name="mono_memory_barrier",input="Pop0",output="Push0",args="InlineI",o1="0xF0",o2="0x10",flow="next"
    case Cil_unused71:      // name="unused71",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x11",flow="next"
    case Cil_unused72:      // name="unused72",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x12",flow="next"
    case Cil_mono_jit_icall_addr:// name="mono_jit_icall_addr",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x13",flow="next"
    case Cil_mono_ldptr_int_req_flag:// name="mono_ldptr_int_req_flag",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x14",flow="next"
    case Cil_mono_ldptr_card_table:// name="mono_ldptr_card_table",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x15",flow="next"
    case Cil_mono_ldptr_nursery_start:// name="mono_ldptr_nursery_start",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x16",flow="next"
    case Cil_mono_ldptr_nursery_bits:// name="mono_ldptr_nursery_bits",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x17",flow="next"
    case Cil_mono_calli_extra_arg:// name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"
    case Cil_mono_lddomain: // name="mono_lddomain",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x19",flow="next"
    case Cil_mono_atomic_store_i4:// name="mono_atomic_store_i4",input="PopI+PopI",output="Push0",args="InlineI",o1="0xF0",o2="0x1A",flow="next"
    case Cil_mono_save_last_error:// name="mono_save_last_error",input="Pop0",output="Push0",args="InlineNone",o1="0xF0",o2="0x1B",flow="next"
    case Cil_mono_get_rgctx_arg:// name="mono_get_rgctx_arg",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x1C",flow="next"
    case Cil_mono_ldptr_profiler_allocation_count:// name="mono_ldptr_profiler_allocation_count",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x1D",flow="next"
    case Cil_mono_ld_delegate_method_ptr:// name="mono_ld_delegate_method_ptr",input="Pop1",output="PushI",args="InlineNone",o1="0xF0",o2="0x1E",flow="next"
    case Cil_mono_rethrow:  // name="mono_rethrow",input="PopRef",output="Push0",args="InlineNone",o1="0xF0",o2="0x1F",flow="throw",type="Objmodel"
    case Cil_mono_get_sp:   // name="mono_get_sp",input="Pop0",output="PushI",args="InlineNone",o1="0xF0",o2="0x20",flow="next"
    case Cil_mono_methodconst:// name="mono_methodconst",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x21",flow="next"
    case Cil_mono_pinvoke_addr_cache:// name="mono_pinvoke_addr_cache",input="Pop0",output="PushI",args="InlineI",o1="0xF0",o2="0x22",flow="next"

  // DQ (11/72021): Added prefixes for instructions (need to check if this is the best way to support these):
    case Cil_constrained:
    case Cil_no:
    case Cil_readonly:
    case Cil_tail:
    case Cil_unaligned:
    case Cil_volatile:
      break;

  // This must be last
    case Cil_last_instruction:
      return false;
    default:
      return false;
  }
    return true;
}

bool
SgAsmCilInstruction::terminatesBasicBlock() {
  switch (get_kind()) {
    case Cil_break:         // name="break",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0x01",flow="break"
    case Cil_jmp:           // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call"
    case Cil_call:          // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call"
    case Cil_calli:         // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call"
    case Cil_ret:           // name="ret",input="VarPop",output="Push0",args="InlineNone",o1="0xFF",o2="0x2A",flow="return"
    case Cil_br_s:          // name="br.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2B",flow="branch"
    case Cil_brfalse_s:     // name="brfalse.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2C",flow="cond-branch"
    case Cil_brtrue_s:      // name="brtrue.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2D",flow="cond-branch"
    case Cil_beq_s:         // name="beq.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2E",flow="cond-branch"
    case Cil_bge_s:         // name="bge.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2F",flow="cond-branch"
    case Cil_bgt_s:         // name="bgt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x30",flow="cond-branch"
    case Cil_ble_s:         // name="ble.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x31",flow="cond-branch"
    case Cil_blt_s:         // name="blt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x32",flow="cond-branch"
    case Cil_bne_un_s:      // name="bne.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x33",flow="cond-branch"
    case Cil_bge_un_s:      // name="bge.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x34",flow="cond-branch"
    case Cil_bgt_un_s:      // name="bgt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x35",flow="cond-branch"
    case Cil_ble_un_s:      // name="ble.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x36",flow="cond-branch"
    case Cil_blt_un_s:      // name="blt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x37",flow="cond-branch"
    case Cil_br:            // name="br",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x38",flow="branch"
    case Cil_brfalse:       // name="brfalse",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x39",flow="cond-branch"
    case Cil_brtrue:        // name="brtrue",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3A",flow="cond-branch"
    case Cil_beq:           // name="beq",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3B",flow="cond-branch"
    case Cil_bge:           // name="bge",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3C",flow="cond-branch"
    case Cil_bgt:           // name="bgt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3D",flow="cond-branch"
    case Cil_ble:           // name="ble",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3E",flow="cond-branch"
    case Cil_blt:           // name="blt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3F",flow="cond-branch"
    case Cil_bne_un:        // name="bne.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x40",flow="cond-branch"
    case Cil_bge_un:        // name="bge.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x41",flow="cond-branch"
    case Cil_bgt_un:        // name="bgt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x42",flow="cond-branch"
    case Cil_ble_un:        // name="ble.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x43",flow="cond-branch"
    case Cil_blt_un:        // name="blt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x44",flow="cond-branch"
    case Cil_switch:        // name="switch",input="PopI",output="Push0",args="InlineSwitch",o1="0xFF",o2="0x45",flow="cond-branch"
    case Cil_callvirt:      // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call"
    case Cil_newobj:        // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call"
    case Cil_throw:         // name="throw",input="PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0x7A",flow="throw"
    case Cil_endfinally:    // name="endfinally",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xDC",flow="return"
    case Cil_leave:         // name="leave",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0xDD",flow="branch"
    case Cil_leave_s:       // name="leave.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0xDE",flow="branch"
    case Cil_endfilter:     // name="endfilter",input="PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x11",flow="return"
    case Cil_rethrow:       // name="rethrow",input="Pop0",output="Push0",args="InlineNone",o1="0xFE",o2="0x1A",flow="throw"
    case Cil_mono_retobj:   // name="mono_retobj",input="PopI",output="Push0",args="InlineType",o1="0xF0",o2="0x05",flow="return"
    case Cil_mono_ldnativeobj:// name="mono_ldnativeobj",input="PopI",output="Push1",args="InlineType",o1="0xF0",o2="0x06",flow="return"
    case Cil_mono_calli_extra_arg:// name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"
    case Cil_mono_rethrow:  // name="mono_rethrow",input="PopRef",output="Push0",args="InlineNone",o1="0xF0",o2="0x1F",flow="throw",type="Objmodel"
      return true;

    default:
      return false;
  }
  return false;
}

bool
SgAsmCilInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va)
{
  SgAsmCilInstruction* last{nullptr};

  if (!insns.empty() && (last=isSgAsmCilInstruction(insns.back()))) {
    // Quick method based only on the kind of instruction
    switch (last->get_kind()) {
      case Cil_jmp:       // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call"
      case Cil_call:      // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call"
      case Cil_calli:     // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call"
      case Cil_callvirt:  // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call"
      case Cil_newobj:    // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call"
      case Cil_mono_calli_extra_arg: // name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"

        if (target) {
          last->branchTarget().assignTo(*target);
        }
        if (return_va) {
          *return_va = last->get_address() + last->get_size();
        }
        return true;

      default:
        return false;
    }
  }

  return false;
}

bool
SgAsmCilInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t* target, rose_addr_t* return_va) {
    return isFunctionCallFast(insns, target, return_va);
}

bool
SgAsmCilInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
  auto iCil = isSgAsmCilInstruction(insns.back());
  if (iCil) {
    switch (iCil->get_kind()) {
      case Cil_ret:             // name="ret",input="VarPop",output="Push0",args="InlineNone",o1="0xFF",o2="0x2A",flow="return"
      case Cil_endfinally:      // name="endfinally",input="Pop0",output="Push0",args="InlineNone",o1="0xFF",o2="0xDC",flow="return"
      case Cil_endfilter:       // name="endfilter",input="PopI",output="Push0",args="InlineNone",o1="0xFE",o2="0x11",flow="return"
      case Cil_mono_retobj:     // name="mono_retobj",input="PopI",output="Push0",args="InlineType",o1="0xF0",o2="0x05",flow="return"
      case Cil_mono_ldnativeobj:// name="mono_ldnativeobj",input="PopI",output="Push1",args="InlineType",o1="0xF0",o2="0x06",flow="return"
          return true;
      default:
        return false;
    }
  }
  return false;
}

bool
SgAsmCilInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>& insns) {
    return isFunctionReturnFast(insns);
    return false;
}

Sawyer::Optional<rose_addr_t>
SgAsmCilInstruction::branchTarget() {
  CilInstructionKind kind = get_kind();

  switch (kind) {
    case Cil_br_s:      // name="br.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2B",flow="branch"
    case Cil_brfalse_s: // name="brfalse.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2C",flow="cond-branch"
    case Cil_brtrue_s:  // name="brtrue.s",input="PopI",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2D",flow="cond-branch"
    case Cil_beq_s:     // name="beq.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2E",flow="cond-branch"
    case Cil_bge_s:     // name="bge.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x2F",flow="cond-branch"
    case Cil_bgt_s:     // name="bgt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x30",flow="cond-branch"
    case Cil_ble_s:     // name="ble.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x31",flow="cond-branch"
    case Cil_blt_s:     // name="blt.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x32",flow="cond-branch"
    case Cil_bne_un_s:  // name="bne.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x33",flow="cond-branch"
    case Cil_bge_un_s:  // name="bge.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x34",flow="cond-branch"
    case Cil_bgt_un_s:  // name="bgt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x35",flow="cond-branch"
    case Cil_ble_un_s:  // name="ble.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x36",flow="cond-branch"
    case Cil_blt_un_s:  // name="blt.un.s",input="Pop1+Pop1",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0x37",flow="cond-branch"
    case Cil_br:        // name="br",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x38",flow="branch"
    case Cil_brfalse:   // name="brfalse",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x39",flow="cond-branch"
    case Cil_brtrue:    // name="brtrue",input="PopI",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3A",flow="cond-branch"
    case Cil_beq:       // name="beq",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3B",flow="cond-branch"
    case Cil_bge:       // name="bge",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3C",flow="cond-branch"
    case Cil_bgt:       // name="bgt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3D",flow="cond-branch"
    case Cil_ble:       // name="ble",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3E",flow="cond-branch"
    case Cil_blt:       // name="blt",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x3F",flow="cond-branch"
    case Cil_bne_un:    // name="bne.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x40",flow="cond-branch"
    case Cil_bge_un:    // name="bge.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x41",flow="cond-branch"
    case Cil_bgt_un:    // name="bgt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x42",flow="cond-branch"
    case Cil_ble_un:    // name="ble.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x43",flow="cond-branch"
    case Cil_blt_un:    // name="blt.un",input="Pop1+Pop1",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0x44",flow="cond-branch"
    case Cil_leave:     // name="leave",input="Pop0",output="Push0",args="InlineBrTarget",o1="0xFF",o2="0xDD",flow="branch"
    case Cil_leave_s:   // name="leave.s",input="Pop0",output="Push0",args="ShortInlineBrTarget",o1="0xFF",o2="0xDE",flow="branch"
      break;

 // A branch instruction but branch target is not immediately available
    case Cil_jmp:       // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call"
    case Cil_call:      // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call"
    case Cil_calli:     // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call"
    case Cil_callvirt:  // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call"
    case Cil_newobj:    // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call"
    case Cil_throw:     // name="throw",input="PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0x7A",flow="throw"
    case Cil_mono_calli_extra_arg: // name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"
    case Cil_mono_rethrow: // name="mono_rethrow",input="PopRef",output="Push0",args="InlineNone",o1="0xF0",o2="0x1F",flow="throw"
      return Sawyer::Nothing();

 // A branch instruction but branch target(s) (offsets) need to be calculated
    case Cil_switch:    // name="switch",input="PopI",output="Push0",args="InlineSwitch",o1="0xFF",o2="0x45",flow="cond-branch"
      return Sawyer::Nothing();

 // Not a branching instruction
    default:
      return Sawyer::Nothing();
  }

  if (nOperands() == 1) {
    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(operand(0))) {
      // The target of a CIL branch instruction is "from the beginning of the instruction following the current instruction"
      return get_address() + get_size() + ival->get_signedValue();
    }
  }
  
  return Sawyer::Nothing();
}

static AddressSet
switchSuccessors(const SgAsmCilInstruction* insn, bool &complete) {
  SgAsmIntegerValueExpression* ival{nullptr};
  AddressSet retval{};
  uint32_t nTargets{0};
  rose_addr_t va{insn->get_address()};
  rose_addr_t fallThrough{va + insn->get_size()};

  complete = false;

  CilInstructionKind kind{insn->get_kind()};
  ASSERT_require(kind == Cil_switch);

  if ((ival = isSgAsmIntegerValueExpression(insn->operand(0)))) {
    nTargets = ival->get_value();
  }
  ASSERT_require(nTargets+1 == insn->nOperands());

  retval.insert(fallThrough);

  for (int n{1}; n < insn->nOperands(); n++) {
    if ((ival = isSgAsmIntegerValueExpression(insn->operand(n)))) {
      retval.insert(fallThrough + ival->get_signedValue());
    }
    else return AddressSet{};
  }

  complete = true;
  return retval;
}

Rose::BinaryAnalysis::AddressSet
SgAsmCilInstruction::getSuccessors(bool &complete) {
  complete = false;
  CilInstructionKind kind = get_kind();

  switch (kind) {
    case Cil_switch:
      return switchSuccessors(this, complete);

 // A branch instruction but branch target is not immediately available
    case Cil_jmp:       // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call"
    case Cil_call:      // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call"
    case Cil_calli:     // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call"
    case Cil_callvirt:  // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call"
    case Cil_newobj:    // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call"
    case Cil_throw:     // name="throw",input="PopRef",output="Push0",args="InlineNone",o1="0xFF",o2="0x7A",flow="throw"
    case Cil_mono_calli_extra_arg: // name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"
    case Cil_mono_rethrow: // name="mono_rethrow",input="PopRef",output="Push0",args="InlineNone",o1="0xF0",o2="0x1F",flow="throw"
      return AddressSet{};

    default:
      break;
  }

  if (auto target{branchTarget()}) {
    AddressSet retval{*target};
    // Add fall through target if not a branch always instruction
    if ((kind != Cil_br) && (kind != Cil_br_s)) {
      retval.insert(get_address() + get_size());
    }
    complete = true;
    return retval;
  }

  return AddressSet{};
}

Rose::BinaryAnalysis::AddressSet
SgAsmCilInstruction::getSuccessors(const std::vector<SgAsmInstruction*>&, bool &complete, const MemoryMap::Ptr&) {
    complete = false;
    return AddressSet();
}

bool
SgAsmCilInstruction::isUnknown() const
{
  return Cil_unknown_instruction == get_kind();
}

unsigned
SgAsmCilInstruction::get_anyKind() const {
 // return (unsigned)null_unknown;
    return p_kind;
}

std::string
SgAsmCilInstruction::description() const
{
  switch (get_kind()) {
    case Cil_unknown_instruction: return "unknown instruction";
    case Cil_nop:       return "do nothing";
    case Cil_break:     return "inform a debugger that a breakpoint has been reached";
    case Cil_ldarg_0:   return "load argument 0 onto the stack";
    case Cil_ldarg_1:
    case Cil_ldarg_2:
    case Cil_ldarg_3:
    case Cil_ldloc_0:
    case Cil_ldloc_1:
    case Cil_ldloc_2:
    case Cil_ldloc_3:
    case Cil_stloc_0:
    case Cil_stloc_1:
    case Cil_stloc_2:
    case Cil_stloc_3:
    case Cil_ldarg_s:
    case Cil_ldarga_s:
    case Cil_starg_s:
    case Cil_ldloc_s:
    case Cil_ldloca_s:
    case Cil_stloc_s:
    case Cil_ldnull:
    case Cil_ldc_i4_m1:
    case Cil_ldc_i4_0:
    case Cil_ldc_i4_1:
    case Cil_ldc_i4_2:
    case Cil_ldc_i4_3:
    case Cil_ldc_i4_4:
    case Cil_ldc_i4_5:
    case Cil_ldc_i4_6:
    case Cil_ldc_i4_7:
    case Cil_ldc_i4_8:
    case Cil_ldc_i4_s:
    case Cil_ldc_i4:
    case Cil_ldc_i8:
    case Cil_ldc_r4:
    case Cil_ldc_r8:
    case Cil_unused99:
    case Cil_dup:
    case Cil_pop:
    case Cil_jmp:
    case Cil_call:
    case Cil_calli:
    case Cil_ret:
    case Cil_br_s:
    case Cil_brfalse_s:
    case Cil_brtrue_s:
    case Cil_beq_s:
    case Cil_bge_s:
    case Cil_bgt_s:
    case Cil_ble_s:
    case Cil_blt_s:
    case Cil_bne_un_s:
    case Cil_bge_un_s:
    case Cil_bgt_un_s:
    case Cil_ble_un_s:
    case Cil_blt_un_s:
    case Cil_br:
    case Cil_brfalse:
    case Cil_brtrue:
    case Cil_beq:
    case Cil_bge:
    case Cil_bgt:
    case Cil_ble:
    case Cil_blt:
    case Cil_bne_un:
    case Cil_bge_un:
    case Cil_bgt_un:
    case Cil_ble_un:
    case Cil_blt_un:
    case Cil_switch:
    case Cil_ldind_i1:
    case Cil_ldind_u1:
    case Cil_ldind_i2:
    case Cil_ldind_u2:
    case Cil_ldind_i4:
    case Cil_ldind_u4:
    case Cil_ldind_i8:
    case Cil_ldind_u8:
    case Cil_ldind_i:
    case Cil_ldind_r4:
    case Cil_ldind_r8:
    case Cil_ldind_ref:
    case Cil_stind_ref:
    case Cil_stind_i1:
    case Cil_stind_i2:
    case Cil_stind_i4:
    case Cil_stind_i8:
    case Cil_stind_r4:
    case Cil_stind_r8:
    case Cil_add:
    case Cil_sub:
    case Cil_mul:
    case Cil_div:
    case Cil_div_un:
    case Cil_rem:
    case Cil_rem_un:
    case Cil_and:
    case Cil_or:
    case Cil_xor:
    case Cil_shl:
    case Cil_shr:
    case Cil_shr_un:
    case Cil_neg:
    case Cil_not:
    case Cil_conv_i1:
    case Cil_conv_i2:
    case Cil_conv_i4:
    case Cil_conv_i8:
    case Cil_conv_r4:
    case Cil_conv_r8:
    case Cil_conv_u4:
    case Cil_conv_u8:
    case Cil_callvirt:
    case Cil_cpobj:
    case Cil_ldobj:
    case Cil_ldstr:
    case Cil_newobj:
    case Cil_castclass:
    case Cil_isinst:
    case Cil_conv_r_un:
    case Cil_unused58:
    case Cil_unused1:
    case Cil_unbox:
    case Cil_throw:
    case Cil_ldfld:
    case Cil_ldflda:
    case Cil_stfld:
    case Cil_ldsfld:
    case Cil_ldsflda:
    case Cil_stsfld:
    case Cil_stobj:
    case Cil_conv_ovf_i1_un:
    case Cil_conv_ovf_i2_un:
    case Cil_conv_ovf_i4_un:
    case Cil_conv_ovf_i8_un:
    case Cil_conv_ovf_u1_un:
    case Cil_conv_ovf_u2_un:
    case Cil_conv_ovf_u4_un:
    case Cil_conv_ovf_u8_un:
    case Cil_conv_ovf_i_un:
    case Cil_conv_ovf_u_un:
    case Cil_box:
    case Cil_newarr:
    case Cil_ldlen:
    case Cil_ldelema:
    case Cil_ldelem_i1:
    case Cil_ldelem_u1:
    case Cil_ldelem_i2:
    case Cil_ldelem_u2:
    case Cil_ldelem_i4:
    case Cil_ldelem_u4:
    case Cil_ldelem_i8:
    case Cil_ldelem_i:
    case Cil_ldelem_r4:
    case Cil_ldelem_r8:
    case Cil_ldelem_ref:
    case Cil_stelem_i:
    case Cil_stelem_i1:
    case Cil_stelem_i2:
    case Cil_stelem_i4:
    case Cil_stelem_i8:
    case Cil_stelem_r4:
    case Cil_stelem_r8:
    case Cil_stelem_ref:
    case Cil_ldelem:
    case Cil_stelem:
    case Cil_unbox_any:
    case Cil_unused5:
    case Cil_unused6:
    case Cil_unused7:
    case Cil_unused8:
    case Cil_unused9:
    case Cil_unused10:
    case Cil_unused11:
    case Cil_unused12:
    case Cil_unused13:
    case Cil_unused14:
    case Cil_unused15:
    case Cil_unused16:
    case Cil_unused17:
    case Cil_conv_ovf_i1:
    case Cil_conv_ovf_u1:
    case Cil_conv_ovf_i2:
    case Cil_conv_ovf_u2:
    case Cil_conv_ovf_i4:
    case Cil_conv_ovf_u4:
    case Cil_conv_ovf_i8:
    case Cil_conv_ovf_u8:
    case Cil_unused50:
    case Cil_unused18:
    case Cil_unused19:
    case Cil_unused20:
    case Cil_unused21:
    case Cil_unused22:
    case Cil_unused23:
    case Cil_refanyval:
    case Cil_ckfinite:
    case Cil_unused24:
    case Cil_unused25:
    case Cil_mkrefany:
    case Cil_unused59:
    case Cil_unused60:
    case Cil_unused61:
    case Cil_unused62:
    case Cil_unused63:
    case Cil_unused64:
    case Cil_unused65:
    case Cil_unused66:
    case Cil_unused67:
    case Cil_ldtoken:
    case Cil_conv_u2:
    case Cil_conv_u1:
    case Cil_conv_i:
    case Cil_conv_ovf_i:
    case Cil_conv_ovf_u:
    case Cil_add_ovf:
    case Cil_add_ovf_un:
    case Cil_mul_ovf:
    case Cil_mul_ovf_un:
    case Cil_sub_ovf:
    case Cil_sub_ovf_un:
    case Cil_endfinally:
    case Cil_leave:
    case Cil_leave_s:
    case Cil_stind_i:
    case Cil_conv_u:
    case Cil_unused26:
    case Cil_unused27:
    case Cil_unused28:
    case Cil_unused29:
    case Cil_unused30:
    case Cil_unused31:
    case Cil_unused32:
    case Cil_unused33:
    case Cil_unused34:
    case Cil_unused35:
    case Cil_unused36:
    case Cil_unused37:
    case Cil_unused38:
    case Cil_unused39:
    case Cil_unused40:
    case Cil_unused41:
    case Cil_unused42:
    case Cil_unused43:
    case Cil_unused44:
    case Cil_unused45:
    case Cil_unused46:
    case Cil_unused47:
    case Cil_unused48:
    case Cil_prefix7:
    case Cil_prefix6:
    case Cil_prefix5:
    case Cil_prefix4:
    case Cil_prefix3:
    case Cil_prefix2:
    case Cil_prefix1:
    case Cil_prefixref:
    case Cil_arglist:
    case Cil_ceq:
    case Cil_cgt:
    case Cil_cgt_un:
    case Cil_clt:
    case Cil_clt_un:
    case Cil_ldftn:
    case Cil_ldvirtftn:
    case Cil_unused56:
    case Cil_ldarg:
    case Cil_ldarga:
    case Cil_starg:
    case Cil_ldloc:
    case Cil_ldloca:
    case Cil_stloc:
    case Cil_localloc:
    case Cil_unused57:
    case Cil_endfilter:
    case Cil_unaligned_:
    case Cil_volatile_:
    case Cil_tail_:
    case Cil_initobj:
    case Cil_constrained_:
    case Cil_cpblk:
    case Cil_initblk:
    case Cil_no_:
    case Cil_rethrow:
    case Cil_unused:
    case Cil_sizeof:
    case Cil_refanytype:
    case Cil_readonly_:
    case Cil_unused53:
    case Cil_unused54:
    case Cil_unused55:
    case Cil_unused70:
    case Cil_illegal:
    case Cil_endmac:
    case Cil_mono_icall:
    case Cil_mono_objaddr:
    case Cil_mono_ldptr:
    case Cil_mono_vtaddr:
    case Cil_mono_newobj:
    case Cil_mono_retobj:
    case Cil_mono_ldnativeobj:
    case Cil_mono_cisinst:
    case Cil_mono_ccastclass:
    case Cil_mono_save_lmf:
    case Cil_mono_restore_lmf:
    case Cil_mono_classconst:
    case Cil_mono_not_taken:
    case Cil_mono_tls:
    case Cil_mono_icall_addr:
    case Cil_mono_dyn_call:
    case Cil_mono_memory_barrier:
    case Cil_unused71:
    case Cil_unused72:
    case Cil_mono_jit_icall_addr:
    case Cil_mono_ldptr_int_req_flag:
    case Cil_mono_ldptr_card_table:
    case Cil_mono_ldptr_nursery_start:
    case Cil_mono_ldptr_nursery_bits:
    case Cil_mono_calli_extra_arg:
    case Cil_mono_lddomain:
    case Cil_mono_atomic_store_i4:
    case Cil_mono_save_last_error:
    case Cil_mono_get_rgctx_arg:
    case Cil_mono_ldptr_profiler_allocation_count:
    case Cil_mono_ld_delegate_method_ptr:
    case Cil_mono_rethrow:
    case Cil_mono_get_sp:
    case Cil_mono_methodconst:
    case Cil_mono_pinvoke_addr_cache:
      return "rose_unimplemented";

  // DQ (11/72021): Added prefixes for instructions (need to check if this is the best way to support these):
    case Cil_constrained:
    case Cil_no:
    case Cil_readonly:
    case Cil_tail:
    case Cil_unaligned:
    case Cil_volatile:
      return "rose_unimplemented";

  // This must be last
    case Cil_last_instruction:
      ASSERT_not_reachable("not a valid CIL instruction kind");
  }
  ASSERT_not_reachable("invalid CIL instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
