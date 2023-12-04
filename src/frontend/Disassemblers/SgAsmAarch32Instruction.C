// SgAsmAarch32Instructoin member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmAarch32Instruction::get_anyKind() const {
    return p_kind;
}

bool
SgAsmAarch32Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t */*target*/,
                                            rose_addr_t */*return_va*/) {
    if (insns.empty())
        return false;

    using Kind = ::Rose::BinaryAnalysis::Aarch32InstructionKind;
    switch (get_kind()) {
        case Kind::ARM_INS_BL:
        case Kind::ARM_INS_BLX:
            return true;
        default:
            return false;
    }
}

bool
SgAsmAarch32Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target,
                                 rose_addr_t *return_va) {
    // FIXME[Robb Matzke 2021-03-02]: For now, just use the fast method
    return isFunctionCallFast(insns, target, return_va);
}

bool
SgAsmAarch32Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
    // Function returns are typically performed by popping the return address from the stack.
    if (insns.empty())
        return false;
    SgAsmAarch32Instruction *insn = isSgAsmAarch32Instruction(insns.back());
    if (!insn)
        return false;
    return insn->get_kind() == Aarch32InstructionKind::ARM_INS_POP && insn->get_writesToIp();
}

bool
SgAsmAarch32Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
    // FIXME[Robb Matzke 2021-03-02]: Not implemented yet. Just use the fast version.
    return isFunctionReturnFast(insns);
}


AddressSet
SgAsmAarch32Instruction::getSuccessors(bool &complete) {
    complete = true;                                    // assume true, and prove otherwise
    ASSERT_forbid(get_condition() == ARM_CC_INVALID);
    using Kind = ::Rose::BinaryAnalysis::Aarch32InstructionKind;
    rose_addr_t fallThroughVa = get_address() + get_size();
    AddressSet retval;
    switch (get_kind()) {
        case Kind::ARM_INS_INVALID:
        case Kind::ARM_INS_ENDING:
            complete = false;
            break;

        //----------------------------------------------------------------------------------------------------
        // Fall through only.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_AESD:
        case Kind::ARM_INS_AESE:
        case Kind::ARM_INS_AESIMC:
        case Kind::ARM_INS_AESMC:
        case Kind::ARM_INS_CLREX:
        case Kind::ARM_INS_CMN:
        case Kind::ARM_INS_CMP:
        case Kind::ARM_INS_CPS:
        case Kind::ARM_INS_DBG:
        case Kind::ARM_INS_DCPS1:
        case Kind::ARM_INS_DCPS2:
        case Kind::ARM_INS_DCPS3:
        case Kind::ARM_INS_DMB:
        case Kind::ARM_INS_DSB:
        case Kind::ARM_INS_ISB:
        case Kind::ARM_INS_LDC:
        case Kind::ARM_INS_MCR:
        case Kind::ARM_INS_MCRR:
        case Kind::ARM_INS_MSR:
        case Kind::ARM_INS_NOP:
        case Kind::ARM_INS_PLD:
        case Kind::ARM_INS_PLDW:
        case Kind::ARM_INS_PLI:
        case Kind::ARM_INS_PUSH:
        case Kind::ARM_INS_SETEND:
        case Kind::ARM_INS_SEV:
        case Kind::ARM_INS_SEVL:
        case Kind::ARM_INS_SRSDA:
        case Kind::ARM_INS_SRSDB:
        case Kind::ARM_INS_SRSIA:
        case Kind::ARM_INS_SRSIB:
        case Kind::ARM_INS_TEQ:
        case Kind::ARM_INS_TST:
        case Kind::ARM_INS_YIELD:
        case Kind::ARM_INS_VABA:
        case Kind::ARM_INS_VABAL:
        case Kind::ARM_INS_VABD:
        case Kind::ARM_INS_VABDL:
        case Kind::ARM_INS_VABS:
        case Kind::ARM_INS_VACGE:
        case Kind::ARM_INS_VACGT:
        case Kind::ARM_INS_VADD:
        case Kind::ARM_INS_VADDHN:
        case Kind::ARM_INS_VADDL:
        case Kind::ARM_INS_VADDW:
        case Kind::ARM_INS_VAND:
        case Kind::ARM_INS_VBIC:
        case Kind::ARM_INS_VBIF:
        case Kind::ARM_INS_VBIT:
        case Kind::ARM_INS_VBSL:
        case Kind::ARM_INS_VCEQ:
        case Kind::ARM_INS_VCGE:
        case Kind::ARM_INS_VCGT:
        case Kind::ARM_INS_VCLE:
        case Kind::ARM_INS_VCLS:
        case Kind::ARM_INS_VCLT:
        case Kind::ARM_INS_VCLZ:
        case Kind::ARM_INS_VCMP:
        case Kind::ARM_INS_VCMPE:
        case Kind::ARM_INS_VCNT:
        case Kind::ARM_INS_VCVT:
        case Kind::ARM_INS_VCVTA:
        case Kind::ARM_INS_VCVTB:
        case Kind::ARM_INS_VCVTM:
        case Kind::ARM_INS_VCVTN:
        case Kind::ARM_INS_VCVTP:
        case Kind::ARM_INS_VCVTR:
        case Kind::ARM_INS_VCVTT:
        case Kind::ARM_INS_VDIV:
        case Kind::ARM_INS_VDUP:
        case Kind::ARM_INS_VEOR:
        case Kind::ARM_INS_VEXT:
        case Kind::ARM_INS_VFMA:
        case Kind::ARM_INS_VFMS:
        case Kind::ARM_INS_VFNMA:
        case Kind::ARM_INS_VFNMS:
        case Kind::ARM_INS_VHADD:
        case Kind::ARM_INS_VHSUB:
        case Kind::ARM_INS_VLDR:
        case Kind::ARM_INS_VMAX:
        case Kind::ARM_INS_VMAXNM:
        case Kind::ARM_INS_VMIN:
        case Kind::ARM_INS_VMINNM:
        case Kind::ARM_INS_VMLA:
        case Kind::ARM_INS_VMLAL:
        case Kind::ARM_INS_VMLS:
        case Kind::ARM_INS_VMLSL:
        case Kind::ARM_INS_VMOVL:
        case Kind::ARM_INS_VMOVN:
        case Kind::ARM_INS_VMUL:
        case Kind::ARM_INS_VMULL:
        case Kind::ARM_INS_VMVN:
        case Kind::ARM_INS_VNEG:
        case Kind::ARM_INS_VNMLA:
        case Kind::ARM_INS_VNMLS:
        case Kind::ARM_INS_VNMUL:
        case Kind::ARM_INS_VORN:
        case Kind::ARM_INS_VORR:
        case Kind::ARM_INS_VPADAL:
        case Kind::ARM_INS_VPADD:
        case Kind::ARM_INS_VPADDL:
        case Kind::ARM_INS_VPMAX:
        case Kind::ARM_INS_VPMIN:
        case Kind::ARM_INS_VQABS:
        case Kind::ARM_INS_VQADD:
        case Kind::ARM_INS_VQDMLAL:
        case Kind::ARM_INS_VQDMLSL:
        case Kind::ARM_INS_VQDMULH:
        case Kind::ARM_INS_VQDMULL:
        case Kind::ARM_INS_VQMOVN:
        case Kind::ARM_INS_VQMOVUN:
        case Kind::ARM_INS_VQNEG:
        case Kind::ARM_INS_VQRDMULH:
        case Kind::ARM_INS_VQRSHL:
        case Kind::ARM_INS_VQRSHRN:
        case Kind::ARM_INS_VQRSHRUN:
        case Kind::ARM_INS_VQSHL:
        case Kind::ARM_INS_VQSHLU:
        case Kind::ARM_INS_VQSHRN:
        case Kind::ARM_INS_VQSHRUN:
        case Kind::ARM_INS_VQSUB:
        case Kind::ARM_INS_VRADDHN:
        case Kind::ARM_INS_VRECPE:
        case Kind::ARM_INS_VRECPS:
        case Kind::ARM_INS_VREV16:
        case Kind::ARM_INS_VREV32:
        case Kind::ARM_INS_VREV64:
        case Kind::ARM_INS_VRHADD:
        case Kind::ARM_INS_VRINTA:
        case Kind::ARM_INS_VRINTM:
        case Kind::ARM_INS_VRINTN:
        case Kind::ARM_INS_VRINTP:
        case Kind::ARM_INS_VRINTR:
        case Kind::ARM_INS_VRINTX:
        case Kind::ARM_INS_VRINTZ:
        case Kind::ARM_INS_VRSHL:
        case Kind::ARM_INS_VRSHR:
        case Kind::ARM_INS_VRSHRN:
        case Kind::ARM_INS_VRSQRTE:
        case Kind::ARM_INS_VRSQRTS:
        case Kind::ARM_INS_VRSRA:
        case Kind::ARM_INS_VRSUBHN:
        case Kind::ARM_INS_VSELEQ:
        case Kind::ARM_INS_VSELGE:
        case Kind::ARM_INS_VSELGT:
        case Kind::ARM_INS_VSELVS:
        case Kind::ARM_INS_VSHL:
        case Kind::ARM_INS_VSHLL:
        case Kind::ARM_INS_VSHR:
        case Kind::ARM_INS_VSHRN:
        case Kind::ARM_INS_VSLI:
        case Kind::ARM_INS_VSQRT:
        case Kind::ARM_INS_VSRA:
        case Kind::ARM_INS_VSRI:
        case Kind::ARM_INS_VSUB:
        case Kind::ARM_INS_VSUBHN:
        case Kind::ARM_INS_VSUBL:
        case Kind::ARM_INS_VSUBW:
        case Kind::ARM_INS_VSWP:
        case Kind::ARM_INS_VTBL:
        case Kind::ARM_INS_VTBX:
        case Kind::ARM_INS_VTRN:
        case Kind::ARM_INS_VTST:
        case Kind::ARM_INS_VUZP:
        case Kind::ARM_INS_VZIP:
            // These instructions always fall through to the next instruction, and never anything else. They cannot write to
            // the IP.
            ASSERT_forbid(get_writesToIp());
            retval.insert(fallThroughVa);
            break;

        //----------------------------------------------------------------------------------------------------
        // Fall through, or UNPREDICTABLE behavior.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_ADC:
        case Kind::ARM_INS_BFC:
        case Kind::ARM_INS_BFI:
        case Kind::ARM_INS_CLZ:
        case Kind::ARM_INS_CRC32B:
        case Kind::ARM_INS_CRC32H:
        case Kind::ARM_INS_CRC32W:
        case Kind::ARM_INS_CRC32CB:
        case Kind::ARM_INS_CRC32CH:
        case Kind::ARM_INS_CRC32CW:
        case Kind::ARM_INS_FLDMDBX:
        case Kind::ARM_INS_FLDMIAX:
        case Kind::ARM_INS_FSTMDBX:
        case Kind::ARM_INS_FSTMIAX:
        case Kind::ARM_INS_LDA:
        case Kind::ARM_INS_LDAB:
        case Kind::ARM_INS_LDAEX:
        case Kind::ARM_INS_LDAEXB:
        case Kind::ARM_INS_LDAEXD:
        case Kind::ARM_INS_LDAEXH:
        case Kind::ARM_INS_LDAH:
        case Kind::ARM_INS_LDRBT:
        case Kind::ARM_INS_LDRD:
        case Kind::ARM_INS_LDREX:
        case Kind::ARM_INS_LDREXB:
        case Kind::ARM_INS_LDREXD:
        case Kind::ARM_INS_LDREXH:
        case Kind::ARM_INS_LDRH:
        case Kind::ARM_INS_LDRHT:
        case Kind::ARM_INS_LDRSB:
        case Kind::ARM_INS_LDRSBT:
        case Kind::ARM_INS_LDRSH:
        case Kind::ARM_INS_LDRSHT:
        case Kind::ARM_INS_LDRT:
        case Kind::ARM_INS_MLA:
        case Kind::ARM_INS_MLS:
        case Kind::ARM_INS_MOVT:
        case Kind::ARM_INS_MRRC:
        case Kind::ARM_INS_MRS:
        case Kind::ARM_INS_MUL:
        case Kind::ARM_INS_ORN:
        case Kind::ARM_INS_PKHBT:
        case Kind::ARM_INS_PKHTB:
        case Kind::ARM_INS_QADD:
        case Kind::ARM_INS_QADD16:
        case Kind::ARM_INS_QADD8:
        case Kind::ARM_INS_QASX:
        case Kind::ARM_INS_QDADD:
        case Kind::ARM_INS_QDSUB:
        case Kind::ARM_INS_QSAX:
        case Kind::ARM_INS_QSUB:
        case Kind::ARM_INS_QSUB16:
        case Kind::ARM_INS_QSUB8:
        case Kind::ARM_INS_RBIT:
        case Kind::ARM_INS_REV:
        case Kind::ARM_INS_REV16:
        case Kind::ARM_INS_REVSH:
        case Kind::ARM_INS_SADD16:
        case Kind::ARM_INS_SADD8:
        case Kind::ARM_INS_SASX:
        case Kind::ARM_INS_SBFX:
        case Kind::ARM_INS_SDIV:
        case Kind::ARM_INS_SEL:
        case Kind::ARM_INS_SHA1C:
        case Kind::ARM_INS_SHA1H:
        case Kind::ARM_INS_SHA1M:
        case Kind::ARM_INS_SHA1P:
        case Kind::ARM_INS_SHA1SU0:
        case Kind::ARM_INS_SHA1SU1:
        case Kind::ARM_INS_SHA256H:
        case Kind::ARM_INS_SHA256H2:
        case Kind::ARM_INS_SHA256SU0:
        case Kind::ARM_INS_SHA256SU1:
        case Kind::ARM_INS_SHADD16:
        case Kind::ARM_INS_SHADD8:
        case Kind::ARM_INS_SHASX:
        case Kind::ARM_INS_SHSAX:
        case Kind::ARM_INS_SHSUB16:
        case Kind::ARM_INS_SHSUB8:
        case Kind::ARM_INS_SMLABB:
        case Kind::ARM_INS_SMLABT:
        case Kind::ARM_INS_SMLATB:
        case Kind::ARM_INS_SMLATT:
        case Kind::ARM_INS_SMLAD:
        case Kind::ARM_INS_SMLADX:
        case Kind::ARM_INS_SMLAL:
        case Kind::ARM_INS_SMLALBB:
        case Kind::ARM_INS_SMLALBT:
        case Kind::ARM_INS_SMLALTB:
        case Kind::ARM_INS_SMLALTT:
        case Kind::ARM_INS_SMLALD:
        case Kind::ARM_INS_SMLALDX:
        case Kind::ARM_INS_SMLAWB:
        case Kind::ARM_INS_SMLAWT:
        case Kind::ARM_INS_SMLSD:
        case Kind::ARM_INS_SMLSDX:
        case Kind::ARM_INS_SMLSLD:
        case Kind::ARM_INS_SMLSLDX:
        case Kind::ARM_INS_SMMLA:
        case Kind::ARM_INS_SMMLAR:
        case Kind::ARM_INS_SMMLS:
        case Kind::ARM_INS_SMMLSR:
        case Kind::ARM_INS_SMMUL:
        case Kind::ARM_INS_SMMULR:
        case Kind::ARM_INS_SMUAD:
        case Kind::ARM_INS_SMUADX:
        case Kind::ARM_INS_SMULBB:
        case Kind::ARM_INS_SMULBT:
        case Kind::ARM_INS_SMULTB:
        case Kind::ARM_INS_SMULTT:
        case Kind::ARM_INS_SMULL:
        case Kind::ARM_INS_SMULWB:
        case Kind::ARM_INS_SMULWT:
        case Kind::ARM_INS_SMUSD:
        case Kind::ARM_INS_SMUSDX:
        case Kind::ARM_INS_SSAT:
        case Kind::ARM_INS_SSAT16:
        case Kind::ARM_INS_SSAX:
        case Kind::ARM_INS_SSUB16:
        case Kind::ARM_INS_SSUB8:
        case Kind::ARM_INS_STL:
        case Kind::ARM_INS_STLB:
        case Kind::ARM_INS_STLEX:
        case Kind::ARM_INS_STLEXB:
        case Kind::ARM_INS_STLEXD:
        case Kind::ARM_INS_STLEXH:
        case Kind::ARM_INS_STLH:
        case Kind::ARM_INS_STM:
        case Kind::ARM_INS_STMDA:
        case Kind::ARM_INS_STMDB:
        case Kind::ARM_INS_STMIB:
        case Kind::ARM_INS_STR:
        case Kind::ARM_INS_STRB:
        case Kind::ARM_INS_STRBT:
        case Kind::ARM_INS_STRD:
        case Kind::ARM_INS_STREX:
        case Kind::ARM_INS_STREXB:
        case Kind::ARM_INS_STREXD:
        case Kind::ARM_INS_STREXH:
        case Kind::ARM_INS_STRH:
        case Kind::ARM_INS_STRHT:
        case Kind::ARM_INS_STRT:
        case Kind::ARM_INS_SXTAB:
        case Kind::ARM_INS_SXTAB16:
        case Kind::ARM_INS_SXTAH:
        case Kind::ARM_INS_SXTB:
        case Kind::ARM_INS_SXTB16:
        case Kind::ARM_INS_SXTH:
        case Kind::ARM_INS_UADD16:
        case Kind::ARM_INS_UADD8:
        case Kind::ARM_INS_UASX:
        case Kind::ARM_INS_UBFX:
        case Kind::ARM_INS_UDIV:
        case Kind::ARM_INS_UHADD16:
        case Kind::ARM_INS_UHADD8:
        case Kind::ARM_INS_UHASX:
        case Kind::ARM_INS_UHSAX:
        case Kind::ARM_INS_UHSUB16:
        case Kind::ARM_INS_UHSUB8:
        case Kind::ARM_INS_UMAAL:
        case Kind::ARM_INS_UMLAL:
        case Kind::ARM_INS_UMULL:
        case Kind::ARM_INS_UQADD16:
        case Kind::ARM_INS_UQADD8:
        case Kind::ARM_INS_UQASX:
        case Kind::ARM_INS_UQSAX:
        case Kind::ARM_INS_UQSUB16:
        case Kind::ARM_INS_UQSUB8:
        case Kind::ARM_INS_USAD8:
        case Kind::ARM_INS_USADA8:
        case Kind::ARM_INS_USAT:
        case Kind::ARM_INS_USAT16:
        case Kind::ARM_INS_USAX:
        case Kind::ARM_INS_USUB16:
        case Kind::ARM_INS_USUB8:
        case Kind::ARM_INS_UXTAB:
        case Kind::ARM_INS_UXTAB16:
        case Kind::ARM_INS_UXTAH:
        case Kind::ARM_INS_UXTB:
        case Kind::ARM_INS_UXTB16:
        case Kind::ARM_INS_UXTH:
        case Kind::ARM_INS_VLD1:
        case Kind::ARM_INS_VLD2:
        case Kind::ARM_INS_VLD3:
        case Kind::ARM_INS_VLD4:
        case Kind::ARM_INS_VLDMDB:
        case Kind::ARM_INS_VLDMIA:
        case Kind::ARM_INS_VMOV:
        case Kind::ARM_INS_VMRS:
        case Kind::ARM_INS_VMSR:
        case Kind::ARM_INS_VPOP:
        case Kind::ARM_INS_VPUSH:
        case Kind::ARM_INS_VST1:
        case Kind::ARM_INS_VST2:
        case Kind::ARM_INS_VST3:
        case Kind::ARM_INS_VST4:
        case Kind::ARM_INS_VSTMDB:
        case Kind::ARM_INS_VSTMIA:
        case Kind::ARM_INS_VSTR:
            // These instructions, when writing to the instruction pointer register, have "UNPREDICTABLE" behavior, otherwise
            // they fall through to the next instruction.
            if (get_writesToIp()) {
                complete = false;
            } else {
                retval.insert(fallThroughVa);
            }
            break;

        //----------------------------------------------------------------------------------------------------
        // Fall through, or conditionally branch.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_ADD:
        case Kind::ARM_INS_ADDW:
        case Kind::ARM_INS_ADR:
        case Kind::ARM_INS_AND:
        case Kind::ARM_INS_ASR:
        case Kind::ARM_INS_BIC:
        case Kind::ARM_INS_EOR:
        case Kind::ARM_INS_LDM:
        case Kind::ARM_INS_LDMDA:
        case Kind::ARM_INS_LDMDB:
        case Kind::ARM_INS_LDMIB:
        case Kind::ARM_INS_LDR:
        case Kind::ARM_INS_LDRB:                        // FIXME[Robb Matzke 2021-03-01]: recheck up to here, some might be UNPREDICTABLE instead
        case Kind::ARM_INS_LSL:
        case Kind::ARM_INS_LSR:
        case Kind::ARM_INS_MOV:
        case Kind::ARM_INS_MOVW:
        case Kind::ARM_INS_MRC:
        case Kind::ARM_INS_MVN:
        case Kind::ARM_INS_ORR:
        case Kind::ARM_INS_POP:
        case Kind::ARM_INS_ROR:
        case Kind::ARM_INS_RRX:
        case Kind::ARM_INS_RSB:
        case Kind::ARM_INS_RSC:
        case Kind::ARM_INS_SBC:
        case Kind::ARM_INS_STC:
        case Kind::ARM_INS_SUB:
        case Kind::ARM_INS_SUBW:
            // These instructions fall through, or branch if they write to the IP.
            //
            // The fall through address is normally the only successor. However, if the instruction writes to the instruction
            // pointer register, then the value written is the basis for the next instruction address. We say "basis" because
            // various additional operations are performed on the address before using it as the next instruciton address
            // (e.g., rounding, changing processor modes, etc).
            //
            // Furthermore, if the instruction writes to the instrunction pointer register and is conditionally executed, then
            // it behaves like a conditional branch.
            //
            // Most of the time, we can't know the resulting value written to the instruction pointer register. The only time
            // we do know is when the constant is available as an immediate value in the instruction encoding, either as an
            // absolute address or a IP-relative address.
            if (get_writesToIp()) {
                if (get_condition() == ARM_CC_AL) {
                    // Acting like an unconditional branch.
                    complete = false;                   // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                } else {
                    // Acting like a conditional branch.
                    complete = false;                   // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                    retval.insert(fallThroughVa);
                }
            } else {
                retval.insert(fallThroughVa);
            }
            break;

        //----------------------------------------------------------------------------------------------------
        // Explicit branch (known or unknown target), or fall through if disabled
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_B:
        case Kind::ARM_INS_BL:
        case Kind::ARM_INS_BLX:
        case Kind::ARM_INS_BX:
        case Kind::ARM_INS_BXJ:
        case Kind::ARM_INS_CBNZ:
        case Kind::ARM_INS_CBZ:
        case Kind::ARM_INS_ERET:
        case Kind::ARM_INS_RFEDA:
        case Kind::ARM_INS_RFEDB:
        case Kind::ARM_INS_RFEIA:
        case Kind::ARM_INS_RFEIB:
            // Conditional or unconditional branch instructions
            ASSERT_require(get_writesToIp());
            if (auto target = branchTarget()) {
                retval.insert(*target);
            } else {
                complete = false;
            }
            if (get_condition() != ARM_CC_AL)
                retval.insert(fallThroughVa);
            break;

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to unknown address
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_BKPT:
        case Kind::ARM_INS_HVC:
        case Kind::ARM_INS_SMC:
        case Kind::ARM_INS_SVC:
        case Kind::ARM_INS_TBB:
        case Kind::ARM_INS_TBH:
        case Kind::ARM_INS_UDF:
        case Kind::ARM_INS_WFE:
        case Kind::ARM_INS_WFI:
            // We never know where these instructions go next.
            complete = false;
            break;

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to itself (infininte loop)
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_HLT:
            retval.insert(get_address());
            break;

        //----------------------------------------------------------------------------------------------------
        // Too weird to handle easily.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_IT:
            complete = false;
            break;

        //----------------------------------------------------------------------------------------------------
        // These are undocumented (at least in the documentation I have), so assume nothing.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_CDP:
        case Kind::ARM_INS_CDP2:
        case Kind::ARM_INS_HINT:
        case Kind::ARM_INS_LDC2:
        case Kind::ARM_INS_LDC2L:
        case Kind::ARM_INS_LDCL:
        case Kind::ARM_INS_MCR2:
        case Kind::ARM_INS_MCRR2:
        case Kind::ARM_INS_MRC2:
        case Kind::ARM_INS_MRRC2:
        case Kind::ARM_INS_STC2:
        case Kind::ARM_INS_STC2L:
        case Kind::ARM_INS_STCL:
        case Kind::ARM_INS_SWP:
        case Kind::ARM_INS_SWPB:
        case Kind::ARM_INS_TRAP:
            complete = false;
            break;
    }
    return retval;
}

Sawyer::Optional<rose_addr_t>
SgAsmAarch32Instruction::branchTarget() {
    // Do not modify "target" when returning false.
    using Kind = ::Rose::BinaryAnalysis::Aarch32InstructionKind;
    switch (get_kind()) {
        case Kind::ARM_INS_INVALID:
        case Kind::ARM_INS_ENDING:
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Fall through only.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_AESD:
        case Kind::ARM_INS_AESE:
        case Kind::ARM_INS_AESIMC:
        case Kind::ARM_INS_AESMC:
        case Kind::ARM_INS_CLREX:
        case Kind::ARM_INS_CMN:
        case Kind::ARM_INS_CMP:
        case Kind::ARM_INS_CPS:
        case Kind::ARM_INS_DBG:
        case Kind::ARM_INS_DCPS1:
        case Kind::ARM_INS_DCPS2:
        case Kind::ARM_INS_DCPS3:
        case Kind::ARM_INS_DMB:
        case Kind::ARM_INS_DSB:
        case Kind::ARM_INS_ISB:
        case Kind::ARM_INS_LDC:
        case Kind::ARM_INS_MCR:
        case Kind::ARM_INS_MCRR:
        case Kind::ARM_INS_MSR:
        case Kind::ARM_INS_NOP:
        case Kind::ARM_INS_PLD:
        case Kind::ARM_INS_PLDW:
        case Kind::ARM_INS_PLI:
        case Kind::ARM_INS_PUSH:
        case Kind::ARM_INS_SETEND:
        case Kind::ARM_INS_SEV:
        case Kind::ARM_INS_SEVL:
        case Kind::ARM_INS_SRSDA:
        case Kind::ARM_INS_SRSDB:
        case Kind::ARM_INS_SRSIA:
        case Kind::ARM_INS_SRSIB:
        case Kind::ARM_INS_TEQ:
        case Kind::ARM_INS_TST:
        case Kind::ARM_INS_YIELD:
        case Kind::ARM_INS_VABA:
        case Kind::ARM_INS_VABAL:
        case Kind::ARM_INS_VABD:
        case Kind::ARM_INS_VABDL:
        case Kind::ARM_INS_VABS:
        case Kind::ARM_INS_VACGE:
        case Kind::ARM_INS_VACGT:
        case Kind::ARM_INS_VADD:
        case Kind::ARM_INS_VADDHN:
        case Kind::ARM_INS_VADDL:
        case Kind::ARM_INS_VADDW:
        case Kind::ARM_INS_VAND:
        case Kind::ARM_INS_VBIC:
        case Kind::ARM_INS_VBIF:
        case Kind::ARM_INS_VBIT:
        case Kind::ARM_INS_VBSL:
        case Kind::ARM_INS_VCEQ:
        case Kind::ARM_INS_VCGE:
        case Kind::ARM_INS_VCGT:
        case Kind::ARM_INS_VCLE:
        case Kind::ARM_INS_VCLS:
        case Kind::ARM_INS_VCLT:
        case Kind::ARM_INS_VCLZ:
        case Kind::ARM_INS_VCMP:
        case Kind::ARM_INS_VCMPE:
        case Kind::ARM_INS_VCNT:
        case Kind::ARM_INS_VCVT:
        case Kind::ARM_INS_VCVTA:
        case Kind::ARM_INS_VCVTB:
        case Kind::ARM_INS_VCVTM:
        case Kind::ARM_INS_VCVTN:
        case Kind::ARM_INS_VCVTP:
        case Kind::ARM_INS_VCVTR:
        case Kind::ARM_INS_VCVTT:
        case Kind::ARM_INS_VDIV:
        case Kind::ARM_INS_VDUP:
        case Kind::ARM_INS_VEOR:
        case Kind::ARM_INS_VEXT:
        case Kind::ARM_INS_VFMA:
        case Kind::ARM_INS_VFMS:
        case Kind::ARM_INS_VFNMA:
        case Kind::ARM_INS_VFNMS:
        case Kind::ARM_INS_VHADD:
        case Kind::ARM_INS_VHSUB:
        case Kind::ARM_INS_VLDR:
        case Kind::ARM_INS_VMAX:
        case Kind::ARM_INS_VMAXNM:
        case Kind::ARM_INS_VMIN:
        case Kind::ARM_INS_VMINNM:
        case Kind::ARM_INS_VMLA:
        case Kind::ARM_INS_VMLAL:
        case Kind::ARM_INS_VMLS:
        case Kind::ARM_INS_VMLSL:
        case Kind::ARM_INS_VMOVL:
        case Kind::ARM_INS_VMOVN:
        case Kind::ARM_INS_VMUL:
        case Kind::ARM_INS_VMULL:
        case Kind::ARM_INS_VMVN:
        case Kind::ARM_INS_VNEG:
        case Kind::ARM_INS_VNMLA:
        case Kind::ARM_INS_VNMLS:
        case Kind::ARM_INS_VNMUL:
        case Kind::ARM_INS_VORN:
        case Kind::ARM_INS_VORR:
        case Kind::ARM_INS_VPADAL:
        case Kind::ARM_INS_VPADD:
        case Kind::ARM_INS_VPADDL:
        case Kind::ARM_INS_VPMAX:
        case Kind::ARM_INS_VPMIN:
        case Kind::ARM_INS_VQABS:
        case Kind::ARM_INS_VQADD:
        case Kind::ARM_INS_VQDMLAL:
        case Kind::ARM_INS_VQDMLSL:
        case Kind::ARM_INS_VQDMULH:
        case Kind::ARM_INS_VQDMULL:
        case Kind::ARM_INS_VQMOVN:
        case Kind::ARM_INS_VQMOVUN:
        case Kind::ARM_INS_VQNEG:
        case Kind::ARM_INS_VQRDMULH:
        case Kind::ARM_INS_VQRSHL:
        case Kind::ARM_INS_VQRSHRN:
        case Kind::ARM_INS_VQRSHRUN:
        case Kind::ARM_INS_VQSHL:
        case Kind::ARM_INS_VQSHLU:
        case Kind::ARM_INS_VQSHRN:
        case Kind::ARM_INS_VQSHRUN:
        case Kind::ARM_INS_VQSUB:
        case Kind::ARM_INS_VRADDHN:
        case Kind::ARM_INS_VRECPE:
        case Kind::ARM_INS_VRECPS:
        case Kind::ARM_INS_VREV16:
        case Kind::ARM_INS_VREV32:
        case Kind::ARM_INS_VREV64:
        case Kind::ARM_INS_VRHADD:
        case Kind::ARM_INS_VRINTA:
        case Kind::ARM_INS_VRINTM:
        case Kind::ARM_INS_VRINTN:
        case Kind::ARM_INS_VRINTP:
        case Kind::ARM_INS_VRINTR:
        case Kind::ARM_INS_VRINTX:
        case Kind::ARM_INS_VRINTZ:
        case Kind::ARM_INS_VRSHL:
        case Kind::ARM_INS_VRSHR:
        case Kind::ARM_INS_VRSHRN:
        case Kind::ARM_INS_VRSQRTE:
        case Kind::ARM_INS_VRSQRTS:
        case Kind::ARM_INS_VRSRA:
        case Kind::ARM_INS_VRSUBHN:
        case Kind::ARM_INS_VSELEQ:
        case Kind::ARM_INS_VSELGE:
        case Kind::ARM_INS_VSELGT:
        case Kind::ARM_INS_VSELVS:
        case Kind::ARM_INS_VSHL:
        case Kind::ARM_INS_VSHLL:
        case Kind::ARM_INS_VSHR:
        case Kind::ARM_INS_VSHRN:
        case Kind::ARM_INS_VSLI:
        case Kind::ARM_INS_VSQRT:
        case Kind::ARM_INS_VSRA:
        case Kind::ARM_INS_VSRI:
        case Kind::ARM_INS_VSUB:
        case Kind::ARM_INS_VSUBHN:
        case Kind::ARM_INS_VSUBL:
        case Kind::ARM_INS_VSUBW:
        case Kind::ARM_INS_VSWP:
        case Kind::ARM_INS_VTBL:
        case Kind::ARM_INS_VTBX:
        case Kind::ARM_INS_VTRN:
        case Kind::ARM_INS_VTST:
        case Kind::ARM_INS_VUZP:
        case Kind::ARM_INS_VZIP:
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Fall through, or UNPREDICTABLE behavior.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_ADC:
        case Kind::ARM_INS_BFC:
        case Kind::ARM_INS_BFI:
        case Kind::ARM_INS_CLZ:
        case Kind::ARM_INS_CRC32B:
        case Kind::ARM_INS_CRC32H:
        case Kind::ARM_INS_CRC32W:
        case Kind::ARM_INS_CRC32CB:
        case Kind::ARM_INS_CRC32CH:
        case Kind::ARM_INS_CRC32CW:
        case Kind::ARM_INS_FLDMDBX:
        case Kind::ARM_INS_FLDMIAX:
        case Kind::ARM_INS_FSTMDBX:
        case Kind::ARM_INS_FSTMIAX:
        case Kind::ARM_INS_LDA:
        case Kind::ARM_INS_LDAB:
        case Kind::ARM_INS_LDAEX:
        case Kind::ARM_INS_LDAEXB:
        case Kind::ARM_INS_LDAEXD:
        case Kind::ARM_INS_LDAEXH:
        case Kind::ARM_INS_LDAH:
        case Kind::ARM_INS_LDRBT:
        case Kind::ARM_INS_LDRD:
        case Kind::ARM_INS_LDREX:
        case Kind::ARM_INS_LDREXB:
        case Kind::ARM_INS_LDREXD:
        case Kind::ARM_INS_LDREXH:
        case Kind::ARM_INS_LDRH:
        case Kind::ARM_INS_LDRHT:
        case Kind::ARM_INS_LDRSB:
        case Kind::ARM_INS_LDRSBT:
        case Kind::ARM_INS_LDRSH:
        case Kind::ARM_INS_LDRSHT:
        case Kind::ARM_INS_LDRT:
        case Kind::ARM_INS_MLA:
        case Kind::ARM_INS_MLS:
        case Kind::ARM_INS_MOVT:
        case Kind::ARM_INS_MRRC:
        case Kind::ARM_INS_MRS:
        case Kind::ARM_INS_MUL:
        case Kind::ARM_INS_ORN:
        case Kind::ARM_INS_PKHBT:
        case Kind::ARM_INS_PKHTB:
        case Kind::ARM_INS_QADD:
        case Kind::ARM_INS_QADD16:
        case Kind::ARM_INS_QADD8:
        case Kind::ARM_INS_QASX:
        case Kind::ARM_INS_QDADD:
        case Kind::ARM_INS_QDSUB:
        case Kind::ARM_INS_QSAX:
        case Kind::ARM_INS_QSUB:
        case Kind::ARM_INS_QSUB16:
        case Kind::ARM_INS_QSUB8:
        case Kind::ARM_INS_RBIT:
        case Kind::ARM_INS_REV:
        case Kind::ARM_INS_REV16:
        case Kind::ARM_INS_REVSH:
        case Kind::ARM_INS_SADD16:
        case Kind::ARM_INS_SADD8:
        case Kind::ARM_INS_SASX:
        case Kind::ARM_INS_SBFX:
        case Kind::ARM_INS_SDIV:
        case Kind::ARM_INS_SEL:
        case Kind::ARM_INS_SHA1C:
        case Kind::ARM_INS_SHA1H:
        case Kind::ARM_INS_SHA1M:
        case Kind::ARM_INS_SHA1P:
        case Kind::ARM_INS_SHA1SU0:
        case Kind::ARM_INS_SHA1SU1:
        case Kind::ARM_INS_SHA256H:
        case Kind::ARM_INS_SHA256H2:
        case Kind::ARM_INS_SHA256SU0:
        case Kind::ARM_INS_SHA256SU1:
        case Kind::ARM_INS_SHADD16:
        case Kind::ARM_INS_SHADD8:
        case Kind::ARM_INS_SHASX:
        case Kind::ARM_INS_SHSAX:
        case Kind::ARM_INS_SHSUB16:
        case Kind::ARM_INS_SHSUB8:
        case Kind::ARM_INS_SMLABB:
        case Kind::ARM_INS_SMLABT:
        case Kind::ARM_INS_SMLATB:
        case Kind::ARM_INS_SMLATT:
        case Kind::ARM_INS_SMLAD:
        case Kind::ARM_INS_SMLADX:
        case Kind::ARM_INS_SMLAL:
        case Kind::ARM_INS_SMLALBB:
        case Kind::ARM_INS_SMLALBT:
        case Kind::ARM_INS_SMLALTB:
        case Kind::ARM_INS_SMLALTT:
        case Kind::ARM_INS_SMLALD:
        case Kind::ARM_INS_SMLALDX:
        case Kind::ARM_INS_SMLAWB:
        case Kind::ARM_INS_SMLAWT:
        case Kind::ARM_INS_SMLSD:
        case Kind::ARM_INS_SMLSDX:
        case Kind::ARM_INS_SMLSLD:
        case Kind::ARM_INS_SMLSLDX:
        case Kind::ARM_INS_SMMLA:
        case Kind::ARM_INS_SMMLAR:
        case Kind::ARM_INS_SMMLS:
        case Kind::ARM_INS_SMMLSR:
        case Kind::ARM_INS_SMMUL:
        case Kind::ARM_INS_SMMULR:
        case Kind::ARM_INS_SMUAD:
        case Kind::ARM_INS_SMUADX:
        case Kind::ARM_INS_SMULBB:
        case Kind::ARM_INS_SMULBT:
        case Kind::ARM_INS_SMULTB:
        case Kind::ARM_INS_SMULTT:
        case Kind::ARM_INS_SMULL:
        case Kind::ARM_INS_SMULWB:
        case Kind::ARM_INS_SMULWT:
        case Kind::ARM_INS_SMUSD:
        case Kind::ARM_INS_SMUSDX:
        case Kind::ARM_INS_SSAT:
        case Kind::ARM_INS_SSAT16:
        case Kind::ARM_INS_SSAX:
        case Kind::ARM_INS_SSUB16:
        case Kind::ARM_INS_SSUB8:
        case Kind::ARM_INS_STL:
        case Kind::ARM_INS_STLB:
        case Kind::ARM_INS_STLEX:
        case Kind::ARM_INS_STLEXB:
        case Kind::ARM_INS_STLEXD:
        case Kind::ARM_INS_STLEXH:
        case Kind::ARM_INS_STLH:
        case Kind::ARM_INS_STM:
        case Kind::ARM_INS_STMDA:
        case Kind::ARM_INS_STMDB:
        case Kind::ARM_INS_STMIB:
        case Kind::ARM_INS_STR:
        case Kind::ARM_INS_STRB:
        case Kind::ARM_INS_STRBT:
        case Kind::ARM_INS_STRD:
        case Kind::ARM_INS_STREX:
        case Kind::ARM_INS_STREXB:
        case Kind::ARM_INS_STREXD:
        case Kind::ARM_INS_STREXH:
        case Kind::ARM_INS_STRH:
        case Kind::ARM_INS_STRHT:
        case Kind::ARM_INS_STRT:
        case Kind::ARM_INS_SXTAB:
        case Kind::ARM_INS_SXTAB16:
        case Kind::ARM_INS_SXTAH:
        case Kind::ARM_INS_SXTB:
        case Kind::ARM_INS_SXTB16:
        case Kind::ARM_INS_SXTH:
        case Kind::ARM_INS_UADD16:
        case Kind::ARM_INS_UADD8:
        case Kind::ARM_INS_UASX:
        case Kind::ARM_INS_UBFX:
        case Kind::ARM_INS_UDIV:
        case Kind::ARM_INS_UHADD16:
        case Kind::ARM_INS_UHADD8:
        case Kind::ARM_INS_UHASX:
        case Kind::ARM_INS_UHSAX:
        case Kind::ARM_INS_UHSUB16:
        case Kind::ARM_INS_UHSUB8:
        case Kind::ARM_INS_UMAAL:
        case Kind::ARM_INS_UMLAL:
        case Kind::ARM_INS_UMULL:
        case Kind::ARM_INS_UQADD16:
        case Kind::ARM_INS_UQADD8:
        case Kind::ARM_INS_UQASX:
        case Kind::ARM_INS_UQSAX:
        case Kind::ARM_INS_UQSUB16:
        case Kind::ARM_INS_UQSUB8:
        case Kind::ARM_INS_USAD8:
        case Kind::ARM_INS_USADA8:
        case Kind::ARM_INS_USAT:
        case Kind::ARM_INS_USAT16:
        case Kind::ARM_INS_USAX:
        case Kind::ARM_INS_USUB16:
        case Kind::ARM_INS_USUB8:
        case Kind::ARM_INS_UXTAB:
        case Kind::ARM_INS_UXTAB16:
        case Kind::ARM_INS_UXTAH:
        case Kind::ARM_INS_UXTB:
        case Kind::ARM_INS_UXTB16:
        case Kind::ARM_INS_UXTH:
        case Kind::ARM_INS_VLD1:
        case Kind::ARM_INS_VLD2:
        case Kind::ARM_INS_VLD3:
        case Kind::ARM_INS_VLD4:
        case Kind::ARM_INS_VLDMDB:
        case Kind::ARM_INS_VLDMIA:
        case Kind::ARM_INS_VMOV:
        case Kind::ARM_INS_VMRS:
        case Kind::ARM_INS_VMSR:
        case Kind::ARM_INS_VPOP:
        case Kind::ARM_INS_VPUSH:
        case Kind::ARM_INS_VST1:
        case Kind::ARM_INS_VST2:
        case Kind::ARM_INS_VST3:
        case Kind::ARM_INS_VST4:
        case Kind::ARM_INS_VSTMDB:
        case Kind::ARM_INS_VSTMIA:
        case Kind::ARM_INS_VSTR:
            // These instructions, when writing to the instruction pointer register, have "UNPREDICTABLE" behavior, otherwise
            // they fall through to the next instruction.
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Fall through, or conditionally branch.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_ADD:
        case Kind::ARM_INS_ADDW:
        case Kind::ARM_INS_ADR:
        case Kind::ARM_INS_AND:
        case Kind::ARM_INS_ASR:
        case Kind::ARM_INS_BIC:
        case Kind::ARM_INS_EOR:
        case Kind::ARM_INS_LDM:
        case Kind::ARM_INS_LDMDA:
        case Kind::ARM_INS_LDMDB:
        case Kind::ARM_INS_LDMIB:
        case Kind::ARM_INS_LDR:
        case Kind::ARM_INS_LDRB:                        // FIXME[Robb Matzke 2021-03-01]: recheck up to here, some might be UNPREDICTABLE instead
        case Kind::ARM_INS_LSL:
        case Kind::ARM_INS_LSR:
        case Kind::ARM_INS_MOV:
        case Kind::ARM_INS_MOVW:
        case Kind::ARM_INS_MRC:
        case Kind::ARM_INS_MVN:
        case Kind::ARM_INS_ORR:
        case Kind::ARM_INS_POP:
        case Kind::ARM_INS_ROR:
        case Kind::ARM_INS_RRX:
        case Kind::ARM_INS_RSB:
        case Kind::ARM_INS_RSC:
        case Kind::ARM_INS_SBC:
        case Kind::ARM_INS_STC:
        case Kind::ARM_INS_SUB:
        case Kind::ARM_INS_SUBW:
            // These instructions fall through, or branch if they write to the IP.
            //
            // The fall through address is normally the only successor. However, if the instruction writes to the instruction
            // pointer register, then the value written is the basis for the next instruction address. We say "basis" because
            // various additional operations are performed on the address before using it as the next instruciton address
            // (e.g., rounding, changing processor modes, etc).
            //
            // Furthermore, if the instruction writes to the instrunction pointer register and is conditionally executed, then
            // it behaves like a conditional branch.
            //
            // Most of the time, we can't know the resulting value written to the instruction pointer register. The only time
            // we do know is when the constant is available as an immediate value in the instruction encoding, either as an
            // absolute address or a IP-relative address.
            if (get_writesToIp()) {
                if (get_condition() == ARM_CC_AL) {
                    // Acting like an unconditional branch.
                    // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                    return Sawyer::Nothing();
                } else {
                    // Acting like a conditional branch.
                    // FIXME[Robb Matzke 2021-03-01]: sometimes we might know the address
                    return Sawyer::Nothing();
                }
            }
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Explicit branch to possibly known address, or fall through if disabled
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_B:
        case Kind::ARM_INS_BL:
        case Kind::ARM_INS_BLX:
        case Kind::ARM_INS_CBNZ:
        case Kind::ARM_INS_CBZ:
            // The branch target is the immediate argument, if present. Some of these instructions can do indirect branching,
            // in which case there won't be an immediate argument.
            for (size_t i = 0; i < nOperands(); ++i) {
                if (isSgAsmIntegerValueExpression(operand(i)))
                    return isSgAsmIntegerValueExpression(operand(i))->get_absoluteValue();
            }
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Explicit branch to impossibly known address, or fall through if disabled
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_BX:
        case Kind::ARM_INS_BXJ:
        case Kind::ARM_INS_ERET:
        case Kind::ARM_INS_RFEDA:
        case Kind::ARM_INS_RFEDB:
        case Kind::ARM_INS_RFEIA:
        case Kind::ARM_INS_RFEIB:
            // Conditional or unconditional branch instructions
            ASSERT_require(get_writesToIp());
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to unknown address
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_BKPT:
        case Kind::ARM_INS_HVC:
        case Kind::ARM_INS_SMC:
        case Kind::ARM_INS_SVC:
        case Kind::ARM_INS_TBB:
        case Kind::ARM_INS_TBH:
        case Kind::ARM_INS_UDF:
        case Kind::ARM_INS_WFE:
        case Kind::ARM_INS_WFI:
            // We never know where these instructions go next.
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // Unconditional branch to itself (infininte loop)
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_HLT:
            return get_address();

        //----------------------------------------------------------------------------------------------------
        // Too weird to handle easily.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_IT:
            return Sawyer::Nothing();

        //----------------------------------------------------------------------------------------------------
        // These are undocumented (at least in the documentation I have), so assume nothing.
        //----------------------------------------------------------------------------------------------------
        case Kind::ARM_INS_CDP:
        case Kind::ARM_INS_CDP2:
        case Kind::ARM_INS_HINT:
        case Kind::ARM_INS_LDC2:
        case Kind::ARM_INS_LDC2L:
        case Kind::ARM_INS_LDCL:
        case Kind::ARM_INS_MCR2:
        case Kind::ARM_INS_MCRR2:
        case Kind::ARM_INS_MRC2:
        case Kind::ARM_INS_MRRC2:
        case Kind::ARM_INS_STC2:
        case Kind::ARM_INS_STC2L:
        case Kind::ARM_INS_STCL:
        case Kind::ARM_INS_SWP:
        case Kind::ARM_INS_SWPB:
        case Kind::ARM_INS_TRAP:
            return Sawyer::Nothing();
    }
    ASSERT_not_reachable("insn not handled: " + get_mnemonic());
}

bool
SgAsmAarch32Instruction::terminatesBasicBlock() {
    return get_writesToIp();
}

bool
SgAsmAarch32Instruction::isUnknown() const {
    return Aarch32InstructionKind::ARM_INS_INVALID == get_kind();
}


#endif
