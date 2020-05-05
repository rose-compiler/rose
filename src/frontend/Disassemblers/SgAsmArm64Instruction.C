/* SgAsmArm64Instruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64
#include "sage3basic.h"

#include "Disassembler.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;                   // temporary

unsigned
SgAsmArm64Instruction::get_anyKind() const {
    return p_kind;
}

/* Returns true if the instruction modifies the instruction pointer (r15). */
static bool modifies_ip(SgAsmArm64Instruction *insn) 
{
#if 1
    ASSERT_not_reachable("FIXME[Robb Matzke 2020-04-14]");
#else
    switch (insn->get_kind()) {

        /* Branch instructions */
        case Kind::ARM64_INS_B:
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLX:
        case Kind::ARM64_INS_BX:
        case Kind::ARM64_INS_BXJ:
            return true;

        /* Comparison instructions */
        case Kind::ARM64_INS_CMN:
        case Kind::ARM64_INS_CMP:
        case Kind::ARM64_INS_TEQ:
        case Kind::ARM64_INS_TST:
            return false;

        /* Load multiple registers instructions. Second argument is the set of registers to load.  If the instruction
         * pointer (r15) can be one of them. */
        case Kind::ARM64_INS_LDM:
        case Kind::ARM64_INS_LDMDA:
        case Kind::ARM64_INS_LDMDB:
        case Kind::ARM64_INS_LDMIA:
        case Kind::ARM64_INS_LDMIB: {
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()>=2);
            SgAsmExprListExp *elist = isSgAsmExprListExp(exprs[1]);
            if (!elist) {
                SgAsmUnaryArmSpecialRegisterList *rlist = isSgAsmUnaryArmSpecialRegisterList(exprs[1]);
                ROSE_ASSERT(rlist);
                elist = isSgAsmExprListExp(rlist->get_operand());
                ROSE_ASSERT(elist);
            }
            for (size_t i=0; i<elist->get_expressions().size(); i++) {
                SgAsmRegisterReferenceExpression *reg = isSgAsmRegisterReferenceExpression(elist->get_expressions()[i]);
                ROSE_ASSERT(reg);
                if (reg->get_descriptor().majorNumber()==arm_regclass_gpr && reg->get_descriptor().minorNumber()==15) {
                    return true;
                }
            }
            return false;
        }

        /* Interrupt-causing instructions */
        case Kind::ARM64_INS_BKPT:
        case Kind::ARM64_INS_SWI:
        case Kind::ARM64_INS_UNDEFINED:
            return true;

        /* Other instructions modify the instruction pointer if it's the first (destination) argument. */
        default: {
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            if (exprs.size()>=1) {
                SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(exprs[0]);
                if (rre &&
                    rre->get_descriptor().majorNumber()==arm_regclass_gpr && rre->get_descriptor().minorNumber()==15) {
                    return true;
                }
            }
        }
    }
    return false;
#endif
}

/** Return control flow successors. See base class for full documentation. */
BinaryAnalysis::Disassembler::AddressSet
SgAsmArm64Instruction::getSuccessors(bool *complete) {
#if 1
    ASSERT_not_reachable("FIXME[Robb Matzke 2020-04-14]");
#else
    using Kind = ::Rose::BinaryAnalysis::Arm64InstructionKind;
    BinaryAnalysis::Disassembler::AddressSet retval;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
    *complete = true; /*assume retval is the complete set of successors for now*/

    switch (get_kind()) {
        case Kind::ARM64_INS_B:
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLX:
        case Kind::ARM64_INS_BX: {
            /* Branch target */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmExpression *dest = exprs[0];
            if (isSgAsmValueExpression(dest)) {
                rose_addr_t target_va = SageInterface::getAsmConstant(isSgAsmValueExpression(dest));
                retval.insert(target_va);
            } else {
                /* Could also be a register reference expression, but we don't know the successor in that case. */
                *complete = false;
            }
            
            /* Fall-through address */
            if (get_condition()!=arm_cond_al)
                retval.insert(get_address()+4);
            break;
        }

        case Kind::ARM64_INS_BXJ: {
            /* First argument is the register that holds the next instruction pointer value to use in the case that Jazelle is
             * not available. We only know the successor if the register is the instruction pointer, in which case the
             * successor is the fall-through address. */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(exprs[0]);
            ROSE_ASSERT(rre);
            if (rre->get_descriptor().majorNumber()==arm_regclass_gpr && rre->get_descriptor().minorNumber()==15) {
                retval.insert(get_address()+4);
            } else {
                *complete = false;
            }
            break;
        }
            
        case Kind::ARM64_INS_CMN:
        case Kind::ARM64_INS_CMP:
        case Kind::ARM64_INS_TEQ:
        case Kind::ARM64_INS_TST:
            /* Comparison and test instructions don't ever affect the instruction pointer; they only fall through */
            retval.insert(get_address()+4);
            break;

        case Kind::ARM64_INS_BKPT:
        case Kind::ARM64_INS_SWI:
        case Kind::ARM64_INS_UNDEFINED:
        case Kind::ARM64_INS_INVALID:
            /* No known successors for interrupt-generating instructions */
            break;

        default:
            if (!modifies_ip(this) || get_condition()!=arm_cond_al) {
                retval.insert(get_address()+4);
            } else {
                *complete = false;
            }
            break;
    }
    return retval;
#endif
}

// Does instruction terminate basic block? See base class for full documentation.
bool
SgAsmArm64Instruction::terminatesBasicBlock() {
    if (get_kind()==Arm64InstructionKind::ARM64_INS_INVALID)
        return true;
    return modifies_ip(this);
}

// Determines whether this is the special ARM "unkown" instruction. See base class for documentation.
bool
SgAsmArm64Instruction::isUnknown() const {
    return Arm64InstructionKind::ARM64_INS_INVALID == get_kind();
}

std::string
SgAsmArm64Instruction::description() const {
    using Kind = ::Rose::BinaryAnalysis::Arm64InstructionKind;
    switch (get_kind()) {
        case Kind::ARM64_INS_INVALID:           return "";
        case Kind::ARM64_INS_ABS:               return "absolute value";
        case Kind::ARM64_INS_ADC:               return "add with carry";
        case Kind::ARM64_INS_ADD:               return "add";
        case Kind::ARM64_INS_ADDHN:             return "add returning high narrow to lower";
        case Kind::ARM64_INS_ADDHN2:            return "add returning high narrow to upper";
        case Kind::ARM64_INS_ADDP:              return "add pair of elements";
        case Kind::ARM64_INS_ADDV:              return "add across vector";
        case Kind::ARM64_INS_ADR:               return "form pc-relative address";
        case Kind::ARM64_INS_ADRP:              return "form pc-relative address to 4kB page";
        case Kind::ARM64_INS_AESD:              return "AES single round decryption";
        case Kind::ARM64_INS_AESE:              return "AES single round encryption";
        case Kind::ARM64_INS_AESIMC:            return "AES inverse mix columns";
        case Kind::ARM64_INS_AESMC:             return "AES mix columns";
        case Kind::ARM64_INS_AND:               return "bitwise AND";
        case Kind::ARM64_INS_ASR:               return "arithmetic shift right";
        case Kind::ARM64_INS_AT:                return "address translate";
        case Kind::ARM64_INS_B:                 return "branch";
        case Kind::ARM64_INS_BFM:               return "bitfield move";
        case Kind::ARM64_INS_BFXIL:             return "bifield extract and insert low";
        case Kind::ARM64_INS_BIC:               return "bitwise bit clear";
        case Kind::ARM64_INS_BIF:               return "";
        case Kind::ARM64_INS_BIT:               return "";
        case Kind::ARM64_INS_BL:                return "branch with link";
        case Kind::ARM64_INS_BLR:               return "branch with link to register";
        case Kind::ARM64_INS_BR:                return "branch to register";
        case Kind::ARM64_INS_BRK:               return "breakpoint instruction";
        case Kind::ARM64_INS_BSL:               return "";
        case Kind::ARM64_INS_CBNZ:              return "compare and branch on nonzero";
        case Kind::ARM64_INS_CBZ:               return "compare and branch on zero";
        case Kind::ARM64_INS_CCMN:              return "conditional compare negative";
        case Kind::ARM64_INS_CCMP:              return "conditional compare";
        case Kind::ARM64_INS_CLREX:             return "clear exclusive";
        case Kind::ARM64_INS_CLS:               return "count leading sign bits";
        case Kind::ARM64_INS_CLZ:               return "count leading zero bits";
        case Kind::ARM64_INS_CMEQ:              return "";
        case Kind::ARM64_INS_CMGE:              return "";
        case Kind::ARM64_INS_CMGT:              return "";
        case Kind::ARM64_INS_CMHI:              return "";
        case Kind::ARM64_INS_CMHS:              return "";
        case Kind::ARM64_INS_CMLE:              return "";
        case Kind::ARM64_INS_CMLT:              return "";
        case Kind::ARM64_INS_CMTST:             return "";
        case Kind::ARM64_INS_CNT:               return "";
        case Kind::ARM64_INS_CRC32B:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CB:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CH:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CW:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CX:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32H:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32W:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32X:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CSEL:              return "conditional select";
        case Kind::ARM64_INS_CSINC:             return "conditional select increment";
        case Kind::ARM64_INS_CSINV:             return "conditional select invert";
        case Kind::ARM64_INS_CSNEG:             return "conditional select negation";
        case Kind::ARM64_INS_DCPS1:             return "debug change PE state to EL1";
        case Kind::ARM64_INS_DCPS2:             return "debug change PE state to EL2";
        case Kind::ARM64_INS_DCPS3:             return "debug change PE state to EL3";
        case Kind::ARM64_INS_DMB:               return "data memory barrier";
        case Kind::ARM64_INS_DRPS:              return "debug restore process state";
        case Kind::ARM64_INS_DSB:               return "data synchronization barrier";
        case Kind::ARM64_INS_DUP:               return "";
        case Kind::ARM64_INS_EON:               return "bitwise exclusive OR NOT";
        case Kind::ARM64_INS_EOR:               return "bitwise exclusive OR";
        case Kind::ARM64_INS_ERET:              return "return from exception";
        case Kind::ARM64_INS_EXTR:              return "extract register";
        case Kind::ARM64_INS_EXT:               return "";
        case Kind::ARM64_INS_FABD:              return "";
        case Kind::ARM64_INS_FABS:              return "";
        case Kind::ARM64_INS_FACGE:             return "";
        case Kind::ARM64_INS_FACGT:             return "";
        case Kind::ARM64_INS_FADD:              return "";
        case Kind::ARM64_INS_FADDP:             return "";
        case Kind::ARM64_INS_FCCMP:             return "";
        case Kind::ARM64_INS_FCCMPE:            return "";
        case Kind::ARM64_INS_FCMEQ:             return "";
        case Kind::ARM64_INS_FCMGE:             return "";
        case Kind::ARM64_INS_FCMGT:             return "";
        case Kind::ARM64_INS_FCMLE:             return "";
        case Kind::ARM64_INS_FCMLT:             return "";
        case Kind::ARM64_INS_FCMP:              return "";
        case Kind::ARM64_INS_FCMPE:             return "";
        case Kind::ARM64_INS_FCSEL:             return "";
        case Kind::ARM64_INS_FCVTAS:            return "";
        case Kind::ARM64_INS_FCVTAU:            return "";
        case Kind::ARM64_INS_FCVT:              return "";
        case Kind::ARM64_INS_FCVTL:             return "";
        case Kind::ARM64_INS_FCVTL2:            return "";
        case Kind::ARM64_INS_FCVTMS:            return "";
        case Kind::ARM64_INS_FCVTMU:            return "";
        case Kind::ARM64_INS_FCVTNS:            return "";
        case Kind::ARM64_INS_FCVTNU:            return "";
        case Kind::ARM64_INS_FCVTN:             return "";
        case Kind::ARM64_INS_FCVTN2:            return "";
        case Kind::ARM64_INS_FCVTPS:            return "";
        case Kind::ARM64_INS_FCVTPU:            return "";
        case Kind::ARM64_INS_FCVTXN:            return "";
        case Kind::ARM64_INS_FCVTXN2:           return "";
        case Kind::ARM64_INS_FCVTZS:            return "";
        case Kind::ARM64_INS_FCVTZU:            return "";
        case Kind::ARM64_INS_FDIV:              return "";
        case Kind::ARM64_INS_FMADD:             return "";
        case Kind::ARM64_INS_FMAX:              return "";
        case Kind::ARM64_INS_FMAXNM:            return "";
        case Kind::ARM64_INS_FMAXNMP:           return "";
        case Kind::ARM64_INS_FMAXNMV:           return "";
        case Kind::ARM64_INS_FMAXP:             return "";
        case Kind::ARM64_INS_FMAXV:             return "";
        case Kind::ARM64_INS_FMIN:              return "";
        case Kind::ARM64_INS_FMINNM:            return "";
        case Kind::ARM64_INS_FMINNMP:           return "";
        case Kind::ARM64_INS_FMINNMV:           return "";
        case Kind::ARM64_INS_FMINP:             return "";
        case Kind::ARM64_INS_FMINV:             return "";
        case Kind::ARM64_INS_FMLA:              return "";
        case Kind::ARM64_INS_FMLS:              return "";
        case Kind::ARM64_INS_FMOV:              return "";
        case Kind::ARM64_INS_FMSUB:             return "";
        case Kind::ARM64_INS_FMUL:              return "";
        case Kind::ARM64_INS_FMULX:             return "";
        case Kind::ARM64_INS_FNEG:              return "";
        case Kind::ARM64_INS_FNMADD:            return "";
        case Kind::ARM64_INS_FNMSUB:            return "";
        case Kind::ARM64_INS_FNMUL:             return "";
        case Kind::ARM64_INS_FRECPE:            return "";
        case Kind::ARM64_INS_FRECPS:            return "";
        case Kind::ARM64_INS_FRECPX:            return "";
        case Kind::ARM64_INS_FRINTA:            return "";
        case Kind::ARM64_INS_FRINTI:            return "";
        case Kind::ARM64_INS_FRINTM:            return "";
        case Kind::ARM64_INS_FRINTN:            return "";
        case Kind::ARM64_INS_FRINTP:            return "";
        case Kind::ARM64_INS_FRINTX:            return "";
        case Kind::ARM64_INS_FRINTZ:            return "";
        case Kind::ARM64_INS_FRSQRTE:           return "";
        case Kind::ARM64_INS_FRSQRTS:           return "";
        case Kind::ARM64_INS_FSQRT:             return "";
        case Kind::ARM64_INS_FSUB:              return "";
        case Kind::ARM64_INS_HINT:              return "hint";
        case Kind::ARM64_INS_HLT:               return "halt";
        case Kind::ARM64_INS_HVC:               return "hypervisor call";
        case Kind::ARM64_INS_INS:               return "";
        case Kind::ARM64_INS_ISB:               return "instruction synchronization barrier";
        case Kind::ARM64_INS_LD1:               return "";
        case Kind::ARM64_INS_LD1R:              return "";
        case Kind::ARM64_INS_LD2R:              return "";
        case Kind::ARM64_INS_LD2:               return "";
        case Kind::ARM64_INS_LD3R:              return "";
        case Kind::ARM64_INS_LD3:               return "";
        case Kind::ARM64_INS_LD4:               return "";
        case Kind::ARM64_INS_LD4R:              return "";
        case Kind::ARM64_INS_LDARB:             return "";
        case Kind::ARM64_INS_LDARH:             return "";
        case Kind::ARM64_INS_LDAR:              return "load-acquire register";
        case Kind::ARM64_INS_LDAXP:             return "load-aquire exclusive pair of registers";
        case Kind::ARM64_INS_LDAXR:             return "load-acquire exclusive register";
        case Kind::ARM64_INS_LDAXRB:            return "load-acquire exclusive register byte";
        case Kind::ARM64_INS_LDAXRH:            return "load-acquire exclusive register halfword";
        case Kind::ARM64_INS_LDNP:              return "load pair of registers with non-temporal hint";
        case Kind::ARM64_INS_LDP:               return "load pair of registers";
        case Kind::ARM64_INS_LDPSW:             return "load pair of registers with signed word";
        case Kind::ARM64_INS_LDR:               return "load register";
        case Kind::ARM64_INS_LDRB:              return "load register byte";
        case Kind::ARM64_INS_LDRH:              return "load register halfword";
        case Kind::ARM64_INS_LDRSB:             return "load signed byte";
        case Kind::ARM64_INS_LDRSH:             return "load signed halfword";
        case Kind::ARM64_INS_LDRSW:             return "load register signed word";
        case Kind::ARM64_INS_LDTR:              return "load register";
        case Kind::ARM64_INS_LDTRB:             return "load register byte";
        case Kind::ARM64_INS_LDTRH:             return "load register halfword";
        case Kind::ARM64_INS_LDTRSB:            return "load register signed byte";
        case Kind::ARM64_INS_LDTRSH:            return "";
        case Kind::ARM64_INS_LDTRSW:            return "load register signed word";
        case Kind::ARM64_INS_LDUR:              return "load register";
        case Kind::ARM64_INS_LDURB:             return "load register byte";
        case Kind::ARM64_INS_LDURH:             return "load register halfword";
        case Kind::ARM64_INS_LDURSB:            return "load register signed byte";
        case Kind::ARM64_INS_LDURSH:            return "load register signed halfword";
        case Kind::ARM64_INS_LDURSW:            return "load register signed word";
        case Kind::ARM64_INS_LDXP:              return "load exclusive pair of registers";
        case Kind::ARM64_INS_LDXR:              return "load exclusive register";
        case Kind::ARM64_INS_LDXRB:             return "load exclusive register byte";
        case Kind::ARM64_INS_LDXRH:             return "load exclusive register halfword";
        case Kind::ARM64_INS_LSL:               return "logical shift left";
        case Kind::ARM64_INS_LSR:               return "logical shift right";
        case Kind::ARM64_INS_MADD:              return "multiply-add";
        case Kind::ARM64_INS_MLA:               return "multiply and accumulate";
        case Kind::ARM64_INS_MLS:               return "";
        case Kind::ARM64_INS_MOV:               return "copy";
        case Kind::ARM64_INS_MOVI:              return "";
        case Kind::ARM64_INS_MOVK:              return "copy wide with keep";
        case Kind::ARM64_INS_MOVN:              return "copy wide with NOT";
        case Kind::ARM64_INS_MOVZ:              return "copy wide with zero";
        case Kind::ARM64_INS_MRS:               return "copy from system register";
        case Kind::ARM64_INS_MSR:               return "copy to system register";
        case Kind::ARM64_INS_MSUB:              return "multiply-subtract";
        case Kind::ARM64_INS_MUL:               return "multiply";
        case Kind::ARM64_INS_MVNI:              return "";
        case Kind::ARM64_INS_NEG:               return "negate";
        case Kind::ARM64_INS_NOT:               return "";
        case Kind::ARM64_INS_ORN:               return "bitwise OR NOT";
        case Kind::ARM64_INS_ORR:               return "bitwise OR";
        case Kind::ARM64_INS_PMUL:              return "";
        case Kind::ARM64_INS_PMULL:             return "";
        case Kind::ARM64_INS_PMULL2:            return "";
        case Kind::ARM64_INS_PRFM:              return "prefetch memory";
        case Kind::ARM64_INS_PRFUM:             return "prefetch memory";
        case Kind::ARM64_INS_RADDHN:            return "";
        case Kind::ARM64_INS_RADDHN2:           return "";
        case Kind::ARM64_INS_RBIT:              return "reverse bits";
        case Kind::ARM64_INS_RET:               return "return from subroutine";
        case Kind::ARM64_INS_REV:               return "reverse bytes";
        case Kind::ARM64_INS_REV16:             return "reverse bytes in 16-bit halfwords";
        case Kind::ARM64_INS_REV32:             return "reverse bytes in 32-bit words";
        case Kind::ARM64_INS_REV64:             return "reverse bytes";
        case Kind::ARM64_INS_ROR:               return "rotate right";
        case Kind::ARM64_INS_RSHRN:             return "";
        case Kind::ARM64_INS_RSHRN2:            return "";
        case Kind::ARM64_INS_RSUBHN:            return "";
        case Kind::ARM64_INS_RSUBHN2:           return "";
        case Kind::ARM64_INS_SABA:              return "";
        case Kind::ARM64_INS_SABAL:             return "";
        case Kind::ARM64_INS_SABAL2:            return "";
        case Kind::ARM64_INS_SABD:              return "";
        case Kind::ARM64_INS_SABDL:             return "";
        case Kind::ARM64_INS_SABDL2:            return "";
        case Kind::ARM64_INS_SADALP:            return "";
        case Kind::ARM64_INS_SADDL:             return "";
        case Kind::ARM64_INS_SADDL2:            return "";
        case Kind::ARM64_INS_SADDLP:            return "";
        case Kind::ARM64_INS_SADDLV:            return "";
        case Kind::ARM64_INS_SADDW:             return "";
        case Kind::ARM64_INS_SADDW2:            return "";
        case Kind::ARM64_INS_SBC:               return "subtract with carry";
        case Kind::ARM64_INS_SBFM:              return "signed bitfield move";
        case Kind::ARM64_INS_SCVTF:             return "";
        case Kind::ARM64_INS_SDIV:              return "signed divide";
        case Kind::ARM64_INS_SHA1C:             return "";
        case Kind::ARM64_INS_SHA1H:             return "";
        case Kind::ARM64_INS_SHA1M:             return "";
        case Kind::ARM64_INS_SHA1P:             return "";
        case Kind::ARM64_INS_SHA1SU0:           return "";
        case Kind::ARM64_INS_SHA1SU1:           return "";
        case Kind::ARM64_INS_SHA256H:           return "";
        case Kind::ARM64_INS_SHA256H2:          return "";
        case Kind::ARM64_INS_SHA256SU0:         return "";
        case Kind::ARM64_INS_SHA256SU1:         return "";
        case Kind::ARM64_INS_SHADD:             return "";
        case Kind::ARM64_INS_SHL:               return "";
        case Kind::ARM64_INS_SHLL:              return "";
        case Kind::ARM64_INS_SHLL2:             return "";
        case Kind::ARM64_INS_SHRN:              return "";
        case Kind::ARM64_INS_SHRN2:             return "";
        case Kind::ARM64_INS_SHSUB:             return "";
        case Kind::ARM64_INS_SLI:               return "";
        case Kind::ARM64_INS_SMADDL:            return "signed multiply-add long";
        case Kind::ARM64_INS_SMAX:              return "";
        case Kind::ARM64_INS_SMAXP:             return "";
        case Kind::ARM64_INS_SMAXV:             return "";
        case Kind::ARM64_INS_SMC:               return "supervisor call OS to monitor";
        case Kind::ARM64_INS_SMIN:              return "";
        case Kind::ARM64_INS_SMINP:             return "";
        case Kind::ARM64_INS_SMINV:             return "";
        case Kind::ARM64_INS_SMLAL:             return "signed multiply long and accumulate long";
        case Kind::ARM64_INS_SMLAL2:            return "";
        case Kind::ARM64_INS_SMLSL:             return "";
        case Kind::ARM64_INS_SMLSL2:            return "";
        case Kind::ARM64_INS_SMOV:              return "";
        case Kind::ARM64_INS_SMSUBL:            return "";
        case Kind::ARM64_INS_SMULH:             return "";
        case Kind::ARM64_INS_SMULL:             return "signed multiply long";
        case Kind::ARM64_INS_SMULL2:            return "";
        case Kind::ARM64_INS_SQABS:             return "";
        case Kind::ARM64_INS_SQADD:             return "";
        case Kind::ARM64_INS_SQDMLAL:           return "";
        case Kind::ARM64_INS_SQDMLAL2:          return "";
        case Kind::ARM64_INS_SQDMLSL:           return "";
        case Kind::ARM64_INS_SQDMLSL2:          return "";
        case Kind::ARM64_INS_SQDMULH:           return "";
        case Kind::ARM64_INS_SQDMULL:           return "";
        case Kind::ARM64_INS_SQDMULL2:          return "";
        case Kind::ARM64_INS_SQNEG:             return "";
        case Kind::ARM64_INS_SQRDMULH:          return "";
        case Kind::ARM64_INS_SQRSHL:            return "";
        case Kind::ARM64_INS_SQRSHRN:           return "";
        case Kind::ARM64_INS_SQRSHRN2:          return "";
        case Kind::ARM64_INS_SQRSHRUN:          return "";
        case Kind::ARM64_INS_SQRSHRUN2:         return "";
        case Kind::ARM64_INS_SQSHLU:            return "";
        case Kind::ARM64_INS_SQSHL:             return "";
        case Kind::ARM64_INS_SQSHRN:            return "";
        case Kind::ARM64_INS_SQSHRN2:           return "";
        case Kind::ARM64_INS_SQSHRUN:           return "";
        case Kind::ARM64_INS_SQSHRUN2:          return "";
        case Kind::ARM64_INS_SQSUB:             return "";
        case Kind::ARM64_INS_SQXTN:             return "";
        case Kind::ARM64_INS_SQXTN2:            return "";
        case Kind::ARM64_INS_SQXTUN:            return "";
        case Kind::ARM64_INS_SQXTUN2:           return "";
        case Kind::ARM64_INS_SRHADD:            return "";
        case Kind::ARM64_INS_SRI:               return "";
        case Kind::ARM64_INS_SRSHL:             return "";
        case Kind::ARM64_INS_SRSHR:             return "";
        case Kind::ARM64_INS_SRSRA:             return "";
        case Kind::ARM64_INS_SSHLL:             return "";
        case Kind::ARM64_INS_SSHLL2:            return "";
        case Kind::ARM64_INS_SSHL:              return "";
        case Kind::ARM64_INS_SSHR:              return "";
        case Kind::ARM64_INS_SSRA:              return "";
        case Kind::ARM64_INS_SSUBL2:            return "";
        case Kind::ARM64_INS_SSUBL:             return "";
        case Kind::ARM64_INS_SSUBW:             return "";
        case Kind::ARM64_INS_SSUBW2:            return "";
        case Kind::ARM64_INS_ST1:               return "";
        case Kind::ARM64_INS_ST2:               return "";
        case Kind::ARM64_INS_ST3:               return "";
        case Kind::ARM64_INS_ST4:               return "";
        case Kind::ARM64_INS_STLR:              return "store-release register";
        case Kind::ARM64_INS_STLRB:             return "store-release register byte";
        case Kind::ARM64_INS_STLRH:             return "store-release register halfword";
        case Kind::ARM64_INS_STLXP:             return "store-release exclusive pair of registers";
        case Kind::ARM64_INS_STLXR:             return "store-release exclusive register";
        case Kind::ARM64_INS_STLXRB:            return "store-release exclusive register byte";
        case Kind::ARM64_INS_STLXRH:            return "store-release exclusive register halfword";
        case Kind::ARM64_INS_STNP:              return "store pair of registers with non-temporal hint";
        case Kind::ARM64_INS_STP:               return "store pair of registers";
        case Kind::ARM64_INS_STR:               return "store register";
        case Kind::ARM64_INS_STRB:              return "store register byte";
        case Kind::ARM64_INS_STRH:              return "store register halfword";
        case Kind::ARM64_INS_STTR:              return "store register";
        case Kind::ARM64_INS_STTRB:             return "store register byte";
        case Kind::ARM64_INS_STTRH:             return "store register halfword";
        case Kind::ARM64_INS_STUR:              return "store register";
        case Kind::ARM64_INS_STURB:             return "store register byte";
        case Kind::ARM64_INS_STURH:             return "store register halfword";
        case Kind::ARM64_INS_STXP:              return "store exclusive pair of registers";
        case Kind::ARM64_INS_STXR:              return "store exclusive register";
        case Kind::ARM64_INS_STXRB:             return "store exclusive register byte";
        case Kind::ARM64_INS_STXRH:             return "store exclusive register halfword";
        case Kind::ARM64_INS_SUB:               return "subtract";
        case Kind::ARM64_INS_SUBHN:             return "";
        case Kind::ARM64_INS_SUBHN2:            return "";
        case Kind::ARM64_INS_SUQADD:            return "";
        case Kind::ARM64_INS_SVC:               return "supervisor call app to OS";
        case Kind::ARM64_INS_SYS:               return "";
        case Kind::ARM64_INS_SYSL:              return "";
        case Kind::ARM64_INS_TBL:               return "";
        case Kind::ARM64_INS_TBNZ:              return "test bit and branch if nonzero";
        case Kind::ARM64_INS_TBX:               return "";
        case Kind::ARM64_INS_TBZ:               return "test bit and branch if zero";
        case Kind::ARM64_INS_TRN1:              return "";
        case Kind::ARM64_INS_TRN2:              return "";
        case Kind::ARM64_INS_UABA:              return "";
        case Kind::ARM64_INS_UABAL:             return "";
        case Kind::ARM64_INS_UABAL2:            return "";
        case Kind::ARM64_INS_UABD:              return "";
        case Kind::ARM64_INS_UABDL:             return "";
        case Kind::ARM64_INS_UABDL2:            return "";
        case Kind::ARM64_INS_UADALP:            return "";
        case Kind::ARM64_INS_UADDL:             return "";
        case Kind::ARM64_INS_UADDL2:            return "";
        case Kind::ARM64_INS_UADDLP:            return "";
        case Kind::ARM64_INS_UADDLV:            return "";
        case Kind::ARM64_INS_UADDW:             return "";
        case Kind::ARM64_INS_UADDW2:            return "";
        case Kind::ARM64_INS_UBFM:              return "unsigned bitfield move";
        case Kind::ARM64_INS_UBFX:              return "unsigned bitfield extract";
        case Kind::ARM64_INS_UCVTF:             return "";
        case Kind::ARM64_INS_UDIV:              return "unsigned divide";
        case Kind::ARM64_INS_UHADD:             return "";
        case Kind::ARM64_INS_UHSUB:             return "";
        case Kind::ARM64_INS_UMADDL:            return "unsigned multiply-add long";
        case Kind::ARM64_INS_UMAX:              return "";
        case Kind::ARM64_INS_UMAXP:             return "";
        case Kind::ARM64_INS_UMAXV:             return "";
        case Kind::ARM64_INS_UMIN:              return "";
        case Kind::ARM64_INS_UMINP:             return "";
        case Kind::ARM64_INS_UMINV:             return "";
        case Kind::ARM64_INS_UMLAL:             return "multiply unsigned accumulate long";
        case Kind::ARM64_INS_UMLAL2:            return "";
        case Kind::ARM64_INS_UMLSL:             return "";
        case Kind::ARM64_INS_UMLSL2:            return "";
        case Kind::ARM64_INS_UMOV:              return "";
        case Kind::ARM64_INS_UMSUBL:            return "unsigned multiply-subtract long";
        case Kind::ARM64_INS_UMULH:             return "unsigned multiply high";
        case Kind::ARM64_INS_UMULL:             return "unsigned multiply long";
        case Kind::ARM64_INS_UMULL2:            return "";
        case Kind::ARM64_INS_UQADD:             return "";
        case Kind::ARM64_INS_UQRSHL:            return "";
        case Kind::ARM64_INS_UQRSHRN:           return "";
        case Kind::ARM64_INS_UQRSHRN2:          return "";
        case Kind::ARM64_INS_UQSHL:             return "";
        case Kind::ARM64_INS_UQSHRN:            return "";
        case Kind::ARM64_INS_UQSHRN2:           return "";
        case Kind::ARM64_INS_UQSUB:             return "";
        case Kind::ARM64_INS_UQXTN:             return "";
        case Kind::ARM64_INS_UQXTN2:            return "";
        case Kind::ARM64_INS_URECPE:            return "";
        case Kind::ARM64_INS_URHADD:            return "";
        case Kind::ARM64_INS_URSHL:             return "";
        case Kind::ARM64_INS_URSHR:             return "";
        case Kind::ARM64_INS_URSQRTE:           return "";
        case Kind::ARM64_INS_URSRA:             return "";
        case Kind::ARM64_INS_USHL:              return "";
        case Kind::ARM64_INS_USHLL:             return "";
        case Kind::ARM64_INS_USHLL2:            return "";
        case Kind::ARM64_INS_USHR:              return "";
        case Kind::ARM64_INS_USQADD:            return "";
        case Kind::ARM64_INS_USRA:              return "";
        case Kind::ARM64_INS_USUBL:             return "";
        case Kind::ARM64_INS_USUBL2:            return "";
        case Kind::ARM64_INS_USUBW:             return "";
        case Kind::ARM64_INS_USUBW2:            return "";
        case Kind::ARM64_INS_UZP1:              return "";
        case Kind::ARM64_INS_UZP2:              return "";
        case Kind::ARM64_INS_XTN:               return "";
        case Kind::ARM64_INS_XTN2:              return "";
        case Kind::ARM64_INS_ZIP1:              return "";
        case Kind::ARM64_INS_ZIP2:              return "";
        case Kind::ARM64_INS_ENDING:            ASSERT_not_reachable("invalid ARM instruction kind");
    }
    ASSERT_not_reachable("invalid AArch64 A64 instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
