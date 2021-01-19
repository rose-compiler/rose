// SgAsmAarch32Instructoin member definitions. Do not move these to src/ROSETTA/Grammar/BinaryInstruction.code (or any other
// *.code file) because then they won't be processed as C++ by IDEs.
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>

#include <Disassembler.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmAarch32Instruction::get_anyKind() const {
    return p_kind;
}

bool
SgAsmAarch32Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target,
                                 rose_addr_t *return_va) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target,
                                 rose_addr_t *return_va) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

AddressSet
SgAsmAarch32Instruction::getSuccessors(bool &complete) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::getBranchTarget(rose_addr_t *target) {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::terminatesBasicBlock() {
    ASSERT_not_implemented("[Robb Matzke 2020-12-22]");
}

bool
SgAsmAarch32Instruction::isUnknown() const {
    return Aarch32InstructionKind::ARM_INS_INVALID == get_kind();
}

std::string
SgAsmAarch32Instruction::description() const {
    using Kind = ::Rose::BinaryAnalysis::Aarch32InstructionKind;
    switch (get_kind()) {
        case Kind::ARM_INS_INVALID:     return "";
        case Kind::ARM_INS_ADC:         return "add with carry";
	case Kind::ARM_INS_ADD:         return "add";
        case Kind::ARM_INS_ADR:         return "form PC-relative address";
        case Kind::ARM_INS_AESD:        return "AES single round encryption";
        case Kind::ARM_INS_AESE:        return "AES single round encryption";
	case Kind::ARM_INS_AESIMC:      return "AES inverse mix columns";
        case Kind::ARM_INS_AESMC:       return "AES mix columns";
	case Kind::ARM_INS_ADDW:        return "add";
	case Kind::ARM_INS_AND:         return "bitwise AND";
	case Kind::ARM_INS_ASR:         return "arithmetic shift right"; // alias of MOV, MOVS
	case Kind::ARM_INS_B:           return "branch";
        case Kind::ARM_INS_BFC:         return "bitfield clear";
	case Kind::ARM_INS_BFI:         return "bitfield insert";
	case Kind::ARM_INS_BIC:         return "bitwise bit clear";
	case Kind::ARM_INS_BKPT:        return "breakpoint";
	case Kind::ARM_INS_BL:          return "branch with link";
	case Kind::ARM_INS_BLX:         return "branch with link and exchange";
	case Kind::ARM_INS_BXJ:         return "branch and exchange Jazelle";
	case Kind::ARM_INS_BX:          return "branch and exchange";
	case Kind::ARM_INS_CBNZ:        return "compare and branch on nonzero";
	case Kind::ARM_INS_CBZ:         return "compare and branch on zero";
	//case Kind::ARM_INS_CDP:
	//case Kind::ARM_INS_CDP2:
	case Kind::ARM_INS_CLREX:       return "clear exclusive";
	case Kind::ARM_INS_CLZ:         return "count leading zeros";
	case Kind::ARM_INS_CMN:         return "compare negative";
	case Kind::ARM_INS_CMP:         return "compare";
	case Kind::ARM_INS_CPS:         return "change PE state";
	case Kind::ARM_INS_CRC32B:      return "perform cyclic redundancy check";
	case Kind::ARM_INS_CRC32CB:     return "perform cyclic redundancy check";
	case Kind::ARM_INS_CRC32CH:     return "perform cyclic redundancy check";
	case Kind::ARM_INS_CRC32CW:     return "perform cyclic redundancy check";
	case Kind::ARM_INS_CRC32H:      return "perform cyclic redundancy check";
        case Kind::ARM_INS_CRC32W:      return "perform cyclic redundancy check";
	case Kind::ARM_INS_DBG:         return "no operation";
	case Kind::ARM_INS_DCPS1:       return "debug change PE state to EL1";
        case Kind::ARM_INS_DCPS2:       return "debug change PE state to EL2";
	case Kind::ARM_INS_DCPS3:       return "debug change PE state to EL3";
	case Kind::ARM_INS_DMB:         return "data memory barrier";
	case Kind::ARM_INS_DSB:         return "data synchronization barrier";
	case Kind::ARM_INS_EOR:         return "bitwise exclusive OR";
	case Kind::ARM_INS_ERET:        return "exception return";
	case Kind::ARM_INS_FLDMDBX:     return "load multiple SIMD&FP decrement before";
	case Kind::ARM_INS_FLDMIAX:     return "load multiple SIMD&FP increment after";
	case Kind::ARM_INS_FSTMDBX:     return "store multiple SIMD&FP decrement before";
	case Kind::ARM_INS_FSTMIAX:     return "store multiple SIMD&FP increment after";
	//case Kind::ARM_INS_HINT:
	case Kind::ARM_INS_HLT:         return "halting breakpoint";
	case Kind::ARM_INS_HVC:         return "hypervisor call";
	case Kind::ARM_INS_ISB:         return "instruction synchronization barrier";
	case Kind::ARM_INS_IT:          return "if-then";
	case Kind::ARM_INS_LDA:         return "load-acquire word";
	case Kind::ARM_INS_LDAB:        return "load-acquire byte";
	case Kind::ARM_INS_LDAEX:       return "load-acquire exclusive word";
	case Kind::ARM_INS_LDAEXB:      return "load-acquire exclusive bytes";
	case Kind::ARM_INS_LDAEXD:      return "load-acquire exclusive double-word";
	case Kind::ARM_INS_LDAEXH:      return "load-acquire exclusive half-word";
        case Kind::ARM_INS_LDAH:        return "load-acquire half-word";
	case Kind::ARM_INS_LDC:         return "load data to coprocessor";
        case Kind::ARM_INS_LDC2:        return "load data to coprocessor"; // version 5 and above
	case Kind::ARM_INS_LDC2L:       return "load data to coprocessor long"; // version 5 and above
        case Kind::ARM_INS_LDCL:        return "load data to coprocessor long";
	case Kind::ARM_INS_LDM:         return "load multiple";
	case Kind::ARM_INS_LDMDA:       return "load multiple decrement after";
	case Kind::ARM_INS_LDMDB:       return "load mulitple decrement before";
	case Kind::ARM_INS_LDMIB:       return "load multiple increment before";
	case Kind::ARM_INS_LDR:         return "load register";
	case Kind::ARM_INS_LDRB:        return "load register byte";
	case Kind::ARM_INS_LDRBT:       return "load register byte unprivileged";
	case Kind::ARM_INS_LDRD:        return "load register dual";
	case Kind::ARM_INS_LDREX:       return "load register exclusive";
	case Kind::ARM_INS_LDREXB:      return "load register exclusive byte";
	case Kind::ARM_INS_LDREXD:      return "load register exclusive double-word";
	case Kind::ARM_INS_LDREXH:      return "load register exclusive half-word";
	case Kind::ARM_INS_LDRH:        return "load register half-word";
	case Kind::ARM_INS_LDRHT:       return "load register half-word unprivileged";
	case Kind::ARM_INS_LDRSB:       return "load register signed byte";
	case Kind::ARM_INS_LDRSBT:      return "load register signed byte unprivileged";
	case Kind::ARM_INS_LDRSH:       return "load register signed half-word";
	case Kind::ARM_INS_LDRSHT:      return "load register signed half-word unprivileged";
	case Kind::ARM_INS_LDRT:        return "load register unprivileged";
        case Kind::ARM_INS_LSL:         return "logical shift left"; // alias of MOV, MOVS
	case Kind::ARM_INS_LSR:         return "logical shift right"; // alias of MOV, MOVS
	case Kind::ARM_INS_MCR:         return "move to system register from general-purpose register";
	//case Kind::ARM_INS_MCR2:
	case Kind::ARM_INS_MCRR:        return "move to system register from two general-purpose registers";
	//case Kind::ARM_INS_MCRR2:
	case Kind::ARM_INS_MLA:         return "multiply accumulate";
	case Kind::ARM_INS_MLS:         return "multiply and subtract";
	case Kind::ARM_INS_MOV:         return "move";
	case Kind::ARM_INS_MOVT:        return "move top";
        case Kind::ARM_INS_MOVW:        return "move";
	case Kind::ARM_INS_MRC:         return "move to general-purpose register from system register";
	//case Kind::ARM_INS_MRC2:
	case Kind::ARM_INS_MRRC:        return "move to two general-purpose registers from system register";
	//case Kind::ARM_INS_MRRC2:
	case Kind::ARM_INS_MRS:         return "move special register to general-purpose register";
        case Kind::ARM_INS_MSR:         return "move to special register";
	case Kind::ARM_INS_MUL:         return "multiply";
	case Kind::ARM_INS_MVN:         return "bitwise NOT";
	case Kind::ARM_INS_NOP:         return "no operation";
	case Kind::ARM_INS_ORN:         return "bitwise OR NOT";
	case Kind::ARM_INS_ORR:         return "bitwise OR";
	case Kind::ARM_INS_PKHBT:       return "pack half-word";
	case Kind::ARM_INS_PKHTB:       return "pack half-word";
	case Kind::ARM_INS_PLD:         return "preload data";
	case Kind::ARM_INS_PLDW:        return "preload data";
	case Kind::ARM_INS_PLI:         return "preload instruction";
	case Kind::ARM_INS_POP:         return "pop multiple registers from stack";
	case Kind::ARM_INS_PUSH:        return "push multiple registers to stack";
	case Kind::ARM_INS_QADD:        return "saturating add";
	case Kind::ARM_INS_QADD16:      return "saturating add 16";
	case Kind::ARM_INS_QADD8:       return "saturating add 8";
	case Kind::ARM_INS_QASX:        return "saturating add and exchange";
	case Kind::ARM_INS_QDADD:       return "saturating double and add";
	case Kind::ARM_INS_QDSUB:       return "saturating double and subtract";
	case Kind::ARM_INS_QSAX:        return "saturating subtract and add with exchange";
	case Kind::ARM_INS_QSUB:        return "saturating subtract";
	case Kind::ARM_INS_QSUB16:      return "saturating subtract 16";
	case Kind::ARM_INS_QSUB8:       return "saturating subtract 8";
	case Kind::ARM_INS_RBIT:        return "reverse bits";
	case Kind::ARM_INS_REV:         return "byte-reverse word";
	case Kind::ARM_INS_REV16:       return "byte-reverse packed half-word";
	case Kind::ARM_INS_REVSH:       return "byte-reverse signed half-word";
	case Kind::ARM_INS_RFEDA:       return "return from exception";
	case Kind::ARM_INS_RFEDB:       return "return from exception";
	case Kind::ARM_INS_RFEIA:       return "return from exception";
	case Kind::ARM_INS_RFEIB:       return "return from exception";
	case Kind::ARM_INS_ROR:         return "rotate right"; // alias of MOV, MOVS
        case Kind::ARM_INS_RRX:         return "rotate right with extend"; // alias of MOV, MOVS
	case Kind::ARM_INS_RSB:         return "reverse subtract";
	case Kind::ARM_INS_RSC:         return "reverse subtract with carry";
	case Kind::ARM_INS_SADD16:      return "signed add 16";
	case Kind::ARM_INS_SADD8:       return "signed add 8";
	case Kind::ARM_INS_SASX:        return "signed add and subtract with exchange";
	case Kind::ARM_INS_SBC:         return "subtract with carry";
	case Kind::ARM_INS_SBFX:        return "signed bitfield extract";
	case Kind::ARM_INS_SDIV:        return "signed divide";
	case Kind::ARM_INS_SEL:         return "select bytes";
	case Kind::ARM_INS_SETEND:      return "set endianness";
	case Kind::ARM_INS_SEV:         return "send event";
	case Kind::ARM_INS_SEVL:        return "send event local";
	case Kind::ARM_INS_SHA1C:       return "SHA1 hash update choose";
	case Kind::ARM_INS_SHA1H:       return "SHA1 fixed rotate";
	case Kind::ARM_INS_SHA1M:       return "SHA1 hash update majority";
	case Kind::ARM_INS_SHA1P:       return "SHA1 hash update parity";
	case Kind::ARM_INS_SHA1SU0:     return "SHA1 schedule update 0";
	case Kind::ARM_INS_SHA1SU1:     return "SHA1 schedule udpate 1";
        case Kind::ARM_INS_SHA256H:     return "SHA256 hash update part 1";
        case Kind::ARM_INS_SHA256H2:    return "SHA256 hash update part 2";
        case Kind::ARM_INS_SHA256SU0:   return "SHA256 schedule update 0";
        case Kind::ARM_INS_SHA256SU1:   return "SHA256 schedule update 1";
	case Kind::ARM_INS_SHADD16:     return "signed halving add 16";
	case Kind::ARM_INS_SHADD8:      return "signed halving add 8";
	case Kind::ARM_INS_SHASX:       return "signed halving add and subtract with exchange";
	case Kind::ARM_INS_SHSAX:       return "signed halving subtract and add with exchange";
	case Kind::ARM_INS_SHSUB16:     return "signed halving subtract 16";
	case Kind::ARM_INS_SHSUB8:      return "signed halving subtract 8";
        case Kind::ARM_INS_SMC:         return "secure monitor call";
	case Kind::ARM_INS_SMLABB:      return "signed multiply accumulate (half-words)";
	case Kind::ARM_INS_SMLABT:      return "signed multiply accumulate (lalf-words)";
	case Kind::ARM_INS_SMLAD:       return "signed multiply accumulate dual";
	case Kind::ARM_INS_SMLADX:      return "signed multiply accumulate dual";
	case Kind::ARM_INS_SMLAL:       return "signed multiply accumulate long";
	case Kind::ARM_INS_SMLALBB:     return "signed multiply accumulate long (half-words)";
	case Kind::ARM_INS_SMLALBT:     return "signed multiply accumulate long (half-words)";
	case Kind::ARM_INS_SMLALD:      return "signed multiply accumulate long dual";
	case Kind::ARM_INS_SMLALDX:     return "signed multiply accumulate long dual";
	case Kind::ARM_INS_SMLALTB:     return "signed multiply accumulate (half-words)";
	case Kind::ARM_INS_SMLALTT:     return "signed multiply accumulate (half-words)";
	case Kind::ARM_INS_SMLATB:      return "signed multiply accumulate long (half-words)";
	case Kind::ARM_INS_SMLATT:      return "signed multiply accumulate (half-words)";
	case Kind::ARM_INS_SMLAWB:      return "signed multiply accumulate (word by half-word)";
	case Kind::ARM_INS_SMLAWT:      return "signed multiply accumulate (word by half-word)";
	case Kind::ARM_INS_SMLSD:       return "signed multiply subtract dual";
	case Kind::ARM_INS_SMLSDX:      return "signed multiply subtract dual";
	case Kind::ARM_INS_SMLSLD:      return "signed multiply subtract long dual";
	case Kind::ARM_INS_SMLSLDX:     return "signed multiply subtract long dual";
	case Kind::ARM_INS_SMMLA:       return "signed most significant word multiply accumulate";
	case Kind::ARM_INS_SMMLAR:      return "signed most significant word multiply accumulate";
	case Kind::ARM_INS_SMMLS:       return "signed most significant word multiply subtract";
	case Kind::ARM_INS_SMMLSR:      return "signed most significant word multiply subtract";
	case Kind::ARM_INS_SMMUL:       return "signed most significant word multiply";
	case Kind::ARM_INS_SMMULR:      return "signed most significant word multiply";
	case Kind::ARM_INS_SMUAD:       return "signed dual multiply add";
	case Kind::ARM_INS_SMUADX:      return "signed dual multiply add";
	case Kind::ARM_INS_SMULBB:      return "signed multiply (half-words)";
	case Kind::ARM_INS_SMULBT:      return "signed multiply (half-words)";
	case Kind::ARM_INS_SMULL:       return "signed multiply long";
	case Kind::ARM_INS_SMULTB:      return "signed multiply (half-words)";
	case Kind::ARM_INS_SMULTT:      return "signed multiply (half-words)";
	case Kind::ARM_INS_SMULWB:      return "signed multiply (word by half-word)";
	case Kind::ARM_INS_SMULWT:      return "signed multiply (word by half-word)";
	case Kind::ARM_INS_SMUSD:       return "signed multiply subtract dual";
	case Kind::ARM_INS_SMUSDX:      return "signed multiply subtract dual";
	case Kind::ARM_INS_SRSDA:       return "store return state";
	case Kind::ARM_INS_SRSDB:       return "store return state";
	case Kind::ARM_INS_SRSIA:       return "store return state";
	case Kind::ARM_INS_SRSIB:       return "store return state";
	case Kind::ARM_INS_SSAT:        return "signed saturate";
	case Kind::ARM_INS_SSAT16:      return "signed saturate 16";
	case Kind::ARM_INS_SSAX:        return "signed subtract and add with exchange";
	case Kind::ARM_INS_SSUB16:      return "signed subtract 16";
	case Kind::ARM_INS_SSUB8:       return "signed subtract 8";
	case Kind::ARM_INS_STC:         return "store coprocessor";
	case Kind::ARM_INS_STC2:        return "store coprocessor"; // ARMv5 and above
	case Kind::ARM_INS_STC2L:       return "store coprocessor long"; // ARMv5 and above
	case Kind::ARM_INS_STCL:        return "store coprocessor long";
	case Kind::ARM_INS_STL:         return "store release word";
	case Kind::ARM_INS_STLB:        return "store release byte";
	case Kind::ARM_INS_STLEX:       return "store release exclusive word";
	case Kind::ARM_INS_STLEXB:      return "store release exclusive byte";
	case Kind::ARM_INS_STLEXD:      return "store release exclusive double-word";
	case Kind::ARM_INS_STLEXH:      return "store release exclusive half-word";
	case Kind::ARM_INS_STLH:        return "store release half-word";
	case Kind::ARM_INS_STM:         return "store multiple";
	case Kind::ARM_INS_STMDA:       return "store multiple decrement after";
	case Kind::ARM_INS_STMDB:       return "store multiple decrement before";
	case Kind::ARM_INS_STMIB:       return "store multiple increment before";
	case Kind::ARM_INS_STR:         return "store register";
	case Kind::ARM_INS_STRB:        return "store register byte";
	case Kind::ARM_INS_STRBT:       return "store register byte unprivileged";
	case Kind::ARM_INS_STRD:        return "store register dual";
	case Kind::ARM_INS_STREX:       return "store register exclusive";
	case Kind::ARM_INS_STREXB:      return "store register exclusive byte";
	case Kind::ARM_INS_STREXD:      return "store register exclusive double-word";
	case Kind::ARM_INS_STREXH:      return "store register exclusive half-word";
	case Kind::ARM_INS_STRH:        return "store register half-word";
	case Kind::ARM_INS_STRHT:       return "store register half-word unprivileged";
	case Kind::ARM_INS_STRT:        return "store register unprivileged";
	case Kind::ARM_INS_SUB:         return "subtract";
	case Kind::ARM_INS_SUBW:        return "subtract";
	case Kind::ARM_INS_SVC:         return "supervisor call";
	case Kind::ARM_INS_SWP:         return "swap between registers and memory";
        case Kind::ARM_INS_SWPB:        return "swap byte between registers and memory";
	case Kind::ARM_INS_SXTAB:       return "sign extend and add byte";
	case Kind::ARM_INS_SXTAB16:     return "sign extend and add byte 16";
	case Kind::ARM_INS_SXTAH:       return "sign extend and add half-word";
	case Kind::ARM_INS_SXTB:        return "sign extend byte";
	case Kind::ARM_INS_SXTB16:      return "sign extend byte 16";
	case Kind::ARM_INS_SXTH:        return "sign extend half-word";
	case Kind::ARM_INS_TBB:         return "table branch byte";
	case Kind::ARM_INS_TBH:         return "table branch half-word";
	case Kind::ARM_INS_TEQ:         return "test equivalence";
	//case Kind::ARM_INS_TRAP:
	case Kind::ARM_INS_TST:         return "test";
	case Kind::ARM_INS_UADD16:      return "unsigned add 16";
	case Kind::ARM_INS_UADD8:       return "unsigned add 8";
	case Kind::ARM_INS_UASX:        return "unsigned add and subtract with exchange";
	case Kind::ARM_INS_UBFX:        return "unsigned bitfield extract";
	case Kind::ARM_INS_UDF:         return "permanently undefined";
	case Kind::ARM_INS_UDIV:        return "unsigned divide";
	case Kind::ARM_INS_UHADD16:     return "unsigned halving add 16";
	case Kind::ARM_INS_UHADD8:      return "unsigned halving add 8";
	case Kind::ARM_INS_UHASX:       return "unsigned halving add and subtract with exchange";
	case Kind::ARM_INS_UHSAX:       return "unsigned halving subtract and add with exchange";
	case Kind::ARM_INS_UHSUB16:     return "unsigned halving subtract 16";
	case Kind::ARM_INS_UHSUB8:      return "unsigned halving subtract 8";
	case Kind::ARM_INS_UMAAL:       return "unsigned multiply accumulate accumulate long";
	case Kind::ARM_INS_UMLAL:       return "unsigned multiply accumulate long";
	case Kind::ARM_INS_UMULL:       return "unsigned multiply long";
	case Kind::ARM_INS_UQADD16:     return "unsigned saturating add 16";
	case Kind::ARM_INS_UQADD8:      return "unsigned saturating add 8";
	case Kind::ARM_INS_UQASX:       return "unsigned saturating add and subtract with exchange";
	case Kind::ARM_INS_UQSAX:       return "unsigned saturating subtract and add with exchange";
	case Kind::ARM_INS_UQSUB16:     return "unsigned saturating subtract 16";
	case Kind::ARM_INS_UQSUB8:      return "unsigned saturating subtract 8";
	case Kind::ARM_INS_USAD8:       return "unsigned sum of absolute differences";
	case Kind::ARM_INS_USADA8:      return "unsigned sum of absolute differences 8";
	case Kind::ARM_INS_USAT:        return "unsigned saturate";
	case Kind::ARM_INS_USAT16:      return "unsigned saturate 16";
	case Kind::ARM_INS_USAX:        return "unsigned subtract and add with exchange";
	case Kind::ARM_INS_USUB16:      return "unsigned subtract 16";
	case Kind::ARM_INS_USUB8:       return "unsigned subtract 8";
	case Kind::ARM_INS_UXTAB:       return "unsigned extend and add byte";
	case Kind::ARM_INS_UXTAB16:     return "unsigned extend and add byte 16";
	case Kind::ARM_INS_UXTAH:       return "unsigned extend and add half-word";
	case Kind::ARM_INS_UXTB:        return "unsigned extend byte";
	case Kind::ARM_INS_UXTB16:      return "unsigned extend byte 16";
	case Kind::ARM_INS_UXTH:        return "unsigned extend half-word";
	case Kind::ARM_INS_VABA:        return "vector absolute difference and accumulate";
	case Kind::ARM_INS_VABAL:       return "vector absolute difference and accumulate long";
	case Kind::ARM_INS_VABD:        return "vector absolute difference";
	case Kind::ARM_INS_VABDL:       return "vector absolute difference long";
	case Kind::ARM_INS_VABS:        return "vector absolute";
	case Kind::ARM_INS_VACGE:       return "vector absolute compare greater than or equal";
	case Kind::ARM_INS_VACGT:       return "vector absolute compare greater than";
	case Kind::ARM_INS_VADD:        return "vector add";
	case Kind::ARM_INS_VADDHN:      return "vector add and narrow returning high half";
	case Kind::ARM_INS_VADDL:       return "vector add long";
	case Kind::ARM_INS_VADDW:       return "vector add wide";
	case Kind::ARM_INS_VAND:        return "vector bitwise AND";
	case Kind::ARM_INS_VBIC:        return "vector bitwise bit clear";
	case Kind::ARM_INS_VBIF:        return "vector bitwise insert if false";
	case Kind::ARM_INS_VBIT:        return "vector bitwise insert if true";
	case Kind::ARM_INS_VBSL:        return "vector bitwise select";
	case Kind::ARM_INS_VCEQ:        return "vector compare equal to zero";
	case Kind::ARM_INS_VCGE:        return "vector compare greater than or equal to zero";
	case Kind::ARM_INS_VCGT:        return "vector compare greater than zero";
	case Kind::ARM_INS_VCLE:        return "vector compare less than or equal to zero";
	case Kind::ARM_INS_VCLS:        return "vector count leading sign bits";
	case Kind::ARM_INS_VCLT:        return "vector compare less than zero";
	case Kind::ARM_INS_VCLZ:        return "vector count leading zeros";
	case Kind::ARM_INS_VCMP:        return "vector compare";
	case Kind::ARM_INS_VCMPE:       return "vector compare rasing invalid operation on NaN";
	case Kind::ARM_INS_VCNT:        return "vector count set bits";
	case Kind::ARM_INS_VCVT:        return "vector convert";
	case Kind::ARM_INS_VCVTA:       return "vector convert with round";
	case Kind::ARM_INS_VCVTB:       return "vector convert to/from half-precision in bottom half of single-precision";
	case Kind::ARM_INS_VCVTM:       return "vector convert round towards negative infinity";
	case Kind::ARM_INS_VCVTN:       return "vector convert round towards nearest";
	case Kind::ARM_INS_VCVTP:       return "vector convert round towards positive infinity";
	case Kind::ARM_INS_VCVTR:       return "vector convert FP to integer";
	case Kind::ARM_INS_VCVTT:       return "vector convert to/from half-precision in top half of single-precision";
	case Kind::ARM_INS_VDIV:        return "vector divide";
	case Kind::ARM_INS_VDUP:        return "vector duplicate";
	case Kind::ARM_INS_VEOR:        return "vector bitwise exclusive OR";
	case Kind::ARM_INS_VEXT:        return "vector extract";
	case Kind::ARM_INS_VFMA:        return "vector fused multiply accumulate";
	case Kind::ARM_INS_VFMS:        return "vector fused multiply subtract";
	case Kind::ARM_INS_VFNMA:       return "vector fused negate multiply accumulate";
	case Kind::ARM_INS_VFNMS:       return "vector fused negate multiply subtract";
	case Kind::ARM_INS_VHADD:       return "vector halving add";
	case Kind::ARM_INS_VHSUB:       return "vector halving subtract";
	case Kind::ARM_INS_VLD1:        return "load one-element structure";
	case Kind::ARM_INS_VLD2:        return "load two-element structure";
	case Kind::ARM_INS_VLD3:        return "load three-element structure";
	case Kind::ARM_INS_VLD4:        return "load four-element structure";
	case Kind::ARM_INS_VLDMDB:      return "load multiple SIMD&FP decrement before";
	case Kind::ARM_INS_VLDMIA:      return "load multiple SIMD&FP increment after";
	case Kind::ARM_INS_VLDR:        return "load SIMD&FP register";
	case Kind::ARM_INS_VMAX:        return "vector maximum";
	case Kind::ARM_INS_VMAXNM:      return "vector maximum NaN";
	case Kind::ARM_INS_VMIN:        return "vector minimum";
	case Kind::ARM_INS_VMINNM:      return "vector minimum NaN";
	case Kind::ARM_INS_VMLA:        return "vector multiply accumulate";
	case Kind::ARM_INS_VMLAL:       return "vector multiply accumulate long";
	case Kind::ARM_INS_VMLS:        return "vector multiply subtract";
	case Kind::ARM_INS_VMLSL:       return "vector multiply subtract long";
	case Kind::ARM_INS_VMOV:        return "vector move";
	case Kind::ARM_INS_VMOVL:       return "vector move long";
	case Kind::ARM_INS_VMOVN:       return "vector move and narrow";
	case Kind::ARM_INS_VMRS:        return "move SIMD&FP special register to general-purpose register";
	case Kind::ARM_INS_VMSR:        return "move general-purpose register to SIMD&FP special register";
	case Kind::ARM_INS_VMUL:        return "vector multiply";
	case Kind::ARM_INS_VMULL:       return "vector multiply long";
	case Kind::ARM_INS_VMVN:        return "vector bitwise NOT";
	case Kind::ARM_INS_VNEG:        return "vector negate";
	case Kind::ARM_INS_VNMLA:       return "vector negate multiply accumulate";
	case Kind::ARM_INS_VNMLS:       return "vector negate multiply subtract";
	case Kind::ARM_INS_VNMUL:       return "vector negate multiply";
	case Kind::ARM_INS_VORN:        return "vector bitwise OR NOT";
	case Kind::ARM_INS_VORR:        return "vector bitwise OR";
	case Kind::ARM_INS_VPADAL:      return "vector pairwise add and accumulate long";
	case Kind::ARM_INS_VPADD:       return "vector pairwise add";
	case Kind::ARM_INS_VPADDL:      return "vector pairwise add long";
	case Kind::ARM_INS_VPMAX:       return "vector pairwise maximum";
	case Kind::ARM_INS_VPMIN:       return "vector pairwise minimum";
	//case Kind::ARM_INS_VPOP: -- aliases VLDM*
	//case Kind::ARM_INS_VPUSH: -- aliases VSTM*
	case Kind::ARM_INS_VQABS:       return "vector saturating absolute";
	case Kind::ARM_INS_VQADD:       return "vector saturating add";
	case Kind::ARM_INS_VQDMLAL:     return "vector saturating doubling multiply accumulate long";
	case Kind::ARM_INS_VQDMLSL:     return "vector saturating doubling multiply subtract long";
	case Kind::ARM_INS_VQDMULH:     return "vector saturating doubling multiply returning high half";
	case Kind::ARM_INS_VQDMULL:     return "vector saturating doubling multiply long";
	case Kind::ARM_INS_VQMOVN:      return "vector saturating move and narrow";
	case Kind::ARM_INS_VQMOVUN:     return "vector saturating move and narrow unsigned";
	case Kind::ARM_INS_VQNEG:       return "vector saturating negate";
	case Kind::ARM_INS_VQRDMULH:    return "vector saturating rounding doubling multiply returning high half";
	case Kind::ARM_INS_VQRSHL:      return "vector saturating rounding shift left";
	case Kind::ARM_INS_VQRSHRN:     return "vector saturating rounding shift left narrow";
	case Kind::ARM_INS_VQRSHRUN:    return "vector saturating rounding shift left narrow unsigned";
	case Kind::ARM_INS_VQSHL:       return "vector saturating shift left";
	case Kind::ARM_INS_VQSHLU:      return "vector saturating shift left unsigned";
	case Kind::ARM_INS_VQSHRN:      return "vector saturating shift right narrow";
	case Kind::ARM_INS_VQSHRUN:     return "vector saturating shift right narrow unsigned";
	case Kind::ARM_INS_VQSUB:       return "vector saturating subtract";
	case Kind::ARM_INS_VRADDHN:     return "vector rounding add and narrow returning high half";
	case Kind::ARM_INS_VRECPE:      return "vector reciprocal estimate";
	case Kind::ARM_INS_VRECPS:      return "vector reciprocal step";
	case Kind::ARM_INS_VREV16:      return "vector reverse in half-words";
	case Kind::ARM_INS_VREV32:      return "vector reverse in words";
	case Kind::ARM_INS_VREV64:      return "vector reverse in double-words";
	case Kind::ARM_INS_VRHADD:      return "vector rounding halving add";
	case Kind::ARM_INS_VRINTA:      return "vector round FP to integer towards nearest with ties to away";
	case Kind::ARM_INS_VRINTM:      return "vector round FP to integer towards negative infinity";
	case Kind::ARM_INS_VRINTN:      return "vector round FP to integer to nearest";
	case Kind::ARM_INS_VRINTP:      return "vector round FP to integer towards positive infinity";
	case Kind::ARM_INS_VRINTR:      return "vector round FP to integer";
	case Kind::ARM_INS_VRINTX:      return "vector round FP to integer inexact";
	case Kind::ARM_INS_VRINTZ:      return "vector round FP to integer towards zero";
	case Kind::ARM_INS_VRSHL:       return "vector rounding shift left";
	case Kind::ARM_INS_VRSHR:       return "vector rounding shift right";
	case Kind::ARM_INS_VRSHRN:      return "vector rounding shift right and narrow";
	case Kind::ARM_INS_VRSQRTE:     return "vector reciprocal square root estimate";
	case Kind::ARM_INS_VRSQRTS:     return "vector reciprocal square root step";
	case Kind::ARM_INS_VRSRA:       return "vector rounding shift right and accumulate";
	case Kind::ARM_INS_VRSUBHN:     return "vector rounding subtract and narrow.";
        case Kind::ARM_INS_VSELEQ:      return "FP conditional less than or equal";
	case Kind::ARM_INS_VSELGE:      return "FP conditional greater than or equal";
	case Kind::ARM_INS_VSELGT:      return "FP conditional greater than";
	case Kind::ARM_INS_VSELVS:      return "FP conditional VS";
	case Kind::ARM_INS_VSHL:        return "vector shift left";
	case Kind::ARM_INS_VSHLL:       return "vector shift left long";
	case Kind::ARM_INS_VSHR:        return "vector shift right";
	case Kind::ARM_INS_VSHRN:       return "vector shift right narrow";
	case Kind::ARM_INS_VSLI:        return "vector shift left and insert";
	case Kind::ARM_INS_VSQRT:       return "square root";
	case Kind::ARM_INS_VSRA:        return "vector shift right and accumulate";
	case Kind::ARM_INS_VSRI:        return "vector shift right and insert";
	case Kind::ARM_INS_VST1:        return "store 1-element structure";
	case Kind::ARM_INS_VST2:        return "store 2-element structure";
	case Kind::ARM_INS_VST3:        return "store 3-element structure";
	case Kind::ARM_INS_VST4:        return "store 4-element structure";
	case Kind::ARM_INS_VSTMDB:      return "store multiple SIMD&FP registers decrement before";
	case Kind::ARM_INS_VSTMIA:      return "store multiple SIMD&FP registers increment after";
	case Kind::ARM_INS_VSTR:        return "store SIMD&FP register";
	case Kind::ARM_INS_VSUB:        return "vector subtract";
	case Kind::ARM_INS_VSUBHN:      return "vector subtract and narrow returning high half";
	case Kind::ARM_INS_VSUBL:       return "vector subtract long";
	case Kind::ARM_INS_VSUBW:       return "vector subtract wide";
	case Kind::ARM_INS_VSWP:        return "vector swap";
	case Kind::ARM_INS_VTBL:        return "vector table lookup";
	case Kind::ARM_INS_VTBX:        return "vector table lookup";
	case Kind::ARM_INS_VTRN:        return "vector transpose";
	case Kind::ARM_INS_VTST:        return "vector test bits";
	case Kind::ARM_INS_VUZP:        return "vector unzip";
	case Kind::ARM_INS_VZIP:        return "vector zip";
	case Kind::ARM_INS_WFE:         return "wait for event";
	case Kind::ARM_INS_WFI:         return "wait for interrupt";
	case Kind::ARM_INS_YIELD:       return "yield hint";
    }
    ASSERT_not_reachable("invalid AArch32 instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
