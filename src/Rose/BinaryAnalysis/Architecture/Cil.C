#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Cil.h>

#include <Rose/BinaryAnalysis/Disassembler/Cil.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include <Rose/BinaryAnalysis/Unparser/Cil.h>

#include <SgAsmCilInstruction.h>
#include <SgAsmGenericHeader.h>
#include <SgAsmIntegerValueExpression.h>
#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Cil::Cil()
    : Base("cil", 2, ByteOrder::ORDER_MSB) {}

Cil::~Cil() {}

Cil::Ptr
Cil::instance() {
    return Ptr(new Cil);
}

RegisterDictionary::Ptr
Cil::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instance(name());
    return registerDictionary_.get();
}

bool
Cil::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    return header->get_sectionsByName("CLR Runtime Header").size() > 0;
}

std::string
Cil::instructionDescription(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case Cil_unknown_instruction: return "unknown instruction";
        case Cil_nop:       return "do nothing";
        case Cil_break:     return "inform a debugger that a breakpoint has been reached";
        case Cil_ldarg_0:   return "load argument 0 onto the stack";
        case Cil_ldarg_1:   return "load argument 1 onto the stack";
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
            return "";
        case Cil_blt_s:      return "branch to target if greater than, short form";
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
            return "";
        case Cil_bgt:        return "branch to target if greater than";
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
            return "";
        case Cil_arglist:     return "argument list handle for the current method";
        case Cil_ceq:         return "push 1 (of type int32) if value 1 equals value2, else push 0";
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
            //  case Cil_illegal:
            //  case Cil_endmac:
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
        case Cil_mono_ldptr_prof_alloc_count:
        case Cil_mono_ld_delegate_method_ptr:
        case Cil_mono_rethrow:
        case Cil_mono_get_sp:
        case Cil_mono_methodconst:
        case Cil_mono_pinvoke_addr_cache:
            return "";

            // This must be last
        case Cil_last_instruction:
            ASSERT_not_reachable("not a valid CIL instruction kind");
    }
    ASSERT_not_reachable("invalid CIL instruction kind: " + StringUtility::numberToString(insn->get_kind()));
}

bool
Cil::isUnknown(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);
    return Cil_unknown_instruction == insn->get_kind();
}

bool
Cil::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
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
}

bool
Cil::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) const {
    if (insns.empty())
        return false;
    auto last = isSgAsmCilInstruction(insns.back());
    ASSERT_not_null(last);

    // Quick method based only on the kind of instruction
    switch (last->get_kind()) {
        case Cil_jmp:       // name="jmp",input="Pop0",output="Push0",args="InlineMethod",o1="0xFF",o2="0x27",flow="call"
        case Cil_call:      // name="call",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x28",flow="call"
        case Cil_calli:     // name="calli",input="VarPop",output="VarPush",args="InlineSig",o1="0xFF",o2="0x29",flow="call"
        case Cil_callvirt:  // name="callvirt",input="VarPop",output="VarPush",args="InlineMethod",o1="0xFF",o2="0x6F",flow="call"
        case Cil_newobj:    // name="newobj",input="VarPop",output="PushRef",args="InlineMethod",o1="0xFF",o2="0x73",flow="call"
        case Cil_mono_calli_extra_arg: // name="mono_calli_extra_arg",input="VarPop",output="VarPush",args="InlineSig",o1="0xF0",o2="0x18",flow="call"

            if (target) {
                branchTarget(last).assignTo(*target);
            }
            if (return_va) {
                *return_va = last->get_address() + last->get_size();
            }
            return true;

        default:
            return false;
    }
}

bool
Cil::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) const {
    auto last = isSgAsmCilInstruction(insns.back());
    ASSERT_not_null(last);

    switch (last->get_kind()) {
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

Sawyer::Optional<rose_addr_t>
Cil::branchTarget(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);

    const CilInstructionKind kind = insn->get_kind();
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

    if (insn->nOperands() == 1) {
        if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(0))) {
            // The target of a CIL branch instruction is "from the beginning of the instruction following the current instruction"
            return insn->get_address() + insn->get_size() + ival->get_signedValue();
        }
    }

    return Sawyer::Nothing();
}

static AddressSet
switchSuccessors(const SgAsmCilInstruction* insn, bool &complete) {
    SgAsmIntegerValueExpression* ival{nullptr};
    AddressSet retval{};
    uint32_t nTargets{0};                               // blame a97ec5f7819
    rose_addr_t va{insn->get_address()};
    rose_addr_t fallThrough{va + insn->get_size()};

    complete = false;

    CilInstructionKind kind{insn->get_kind()};          // blame a97ec5f7819
    ASSERT_require(kind == Cil_switch);

    if ((ival = isSgAsmIntegerValueExpression(insn->operand(0)))) {
        nTargets = ival->get_value();
    }
    ASSERT_require(nTargets+1 == insn->nOperands());

    retval.insert(fallThrough);

    for (uint n{1}; n < insn->nOperands(); n++) {
        if ((ival = isSgAsmIntegerValueExpression(insn->operand(n)))) {
            retval.insert(fallThrough + ival->get_signedValue());
        }
        else return AddressSet{};
    }

    complete = true;
    return retval;
}

Rose::BinaryAnalysis::AddressSet
Cil::getSuccessors(SgAsmInstruction *insn_, bool &complete) const {
    auto insn = isSgAsmCilInstruction(insn_);
    ASSERT_not_null(insn);

    complete = false;
    CilInstructionKind kind = insn->get_kind();

    switch (kind) {
        case Cil_switch:
            return switchSuccessors(insn, complete);

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

    if (auto target{branchTarget(insn)}) {
        AddressSet retval{*target};
        // Add fall through target if not a branch always instruction
        if ((kind != Cil_br) && (kind != Cil_br_s)) {
            retval.insert(insn->get_address() + insn->get_size());
        }
        complete = true;
        return retval;
    }

    return AddressSet{};
}

Disassembler::Base::Ptr
Cil::newInstructionDecoder() const {
    return Disassembler::Cil::instance(shared_from_this());
}

Unparser::Base::Ptr
Cil::newUnparser() const {
    return Unparser::Cil::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
