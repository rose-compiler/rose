#include "sage3basic.h"
#include "stringify.h"
#include "AsmUnparser_compat.h"
#include "Registers.h"
#include "Diagnostics.h"

using namespace Rose;
using namespace Diagnostics;
using namespace BinaryAnalysis;

static std::string unparseArmRegister(SgAsmRegisterReferenceExpression *reg, const RegisterDictionary *registers) {
    RegisterDescriptor rdesc = reg->get_descriptor();
    if (!registers)
        registers = RegisterDictionary::dictionary_arm7();
    
    std::string name = registers->lookup(rdesc);
    if (name.empty()) {
        SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(reg);
        name = AsmUnparser::invalid_register(insn, rdesc, registers);
    }

    if (SgAsmDirectRegisterExpression *dre = isSgAsmDirectRegisterExpression(reg)) {
        /* Add mask letters to program status registers */
        if (rdesc.majorNumber()==arm_regclass_psr && dre->get_psr_mask()!=0) {
            name += "_";
            if (dre->get_psr_mask() & 1) name += "c";
            if (dre->get_psr_mask() & 2) name += "x";
            if (dre->get_psr_mask() & 4) name += "s";
            if (dre->get_psr_mask() & 8) name += "f";
        }
    }

    return name;
}

static std::string unparseArmCondition(ArmInstructionCondition cond) { // Unparse as used for mnemonics
#ifndef _MSC_VER
    std::string retval = stringifyBinaryAnalysisArmInstructionCondition(cond, "arm_cond_");
#else
    ROSE_ASSERT(false);
    std::string retval ="";
#endif

    ASSERT_require(retval[0]!='(');
    return retval;
}

static std::string unparseArmSign(ArmSignForExpressionUnparsing sign) {
  switch (sign) {
    case arm_sign_none: return "";
    case arm_sign_plus: return "+";
    case arm_sign_minus: return "-";
    default: {
        ASSERT_not_reachable("invalid ARM sign for expression unparsing: " + stringifyArmSignForExpressionUnparsing(sign));
        // DQ (11/28/2009): MSVC warns about a path that does not have a return stmt.
        return "error in unparseArmSign";
    }
  }
}

/* Helper function for unparseArmExpression(SgAsmExpression*)
 * 
 * If this function is called for an EXPR node that cannot appear at the top of an ARM instruction operand tree then the node
 * might create two strings: the primary expression return value and an additional string returned through the SUFFIX
 * argument. What to do with the additional string depends on layers higher up in the call stack.
 * 
 * The sign will be prepended to the result if EXPR is a value expression of some sort. */
static std::string unparseArmExpression(SgAsmExpression* expr, const AsmUnparser::LabelMap *labels,
                                        const RegisterDictionary *registers,
                                        ArmSignForExpressionUnparsing sign, std::string *suffix=NULL)
{
    std::string result, extra;
    if (!isSgAsmValueExpression(expr)) {
        result += unparseArmSign(sign);
    }
    switch (expr->variantT()) {
        case V_SgAsmBinaryMultiply:
            ASSERT_require(isSgAsmIntegerValueExpression(isSgAsmBinaryExpression(expr)->get_rhs()));
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, arm_sign_none) + "*" +
                     StringUtility::numberToString(isSgAsmIntegerValueExpression(isSgAsmBinaryExpression(expr)->get_rhs()));
            break;
        case V_SgAsmBinaryLsl:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, arm_sign_none) + ", lsl " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, arm_sign_none);
            break;
        case V_SgAsmBinaryLsr:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, arm_sign_none) + ", lsr " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, arm_sign_none);
            break;
        case V_SgAsmBinaryAsr:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, arm_sign_none) + ", asr " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, arm_sign_none);
            break;
        case V_SgAsmBinaryRor:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, arm_sign_none) + ", ror " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, arm_sign_none);
            break;
        case V_SgAsmUnaryRrx:
            result = unparseArmExpression(isSgAsmUnaryExpression(expr)->get_operand(), labels, registers, arm_sign_none) +
                     ", rrx";
            break;
        case V_SgAsmUnaryArmSpecialRegisterList:
            result += unparseArmExpression(isSgAsmUnaryExpression(expr)->get_operand(), labels, registers, arm_sign_none) + "^";
            break;
        case V_SgAsmExprListExp: {
            SgAsmExprListExp* el = isSgAsmExprListExp(expr);
            const std::vector<SgAsmExpression*>& exprs = el->get_expressions();
            result += "{";
            for (size_t i = 0; i < exprs.size(); ++i) {
                if (i != 0) result += ", ";
                result += unparseArmExpression(exprs[i], labels, registers, arm_sign_none);
            }
            result += "}";
            break;
        }


        case V_SgAsmBinaryAdd: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), labels, registers, arm_sign_plus);
            break;
        }

        case V_SgAsmBinarySubtract: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), labels, registers, arm_sign_minus);
            break;
        }

        case V_SgAsmBinaryAddPreupdate: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), labels, registers, arm_sign_plus);
            extra = "!";
            break;
        }
            
        case V_SgAsmBinarySubtractPreupdate: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), labels, registers, arm_sign_minus);
            extra = "!";
            break;
        }

        case V_SgAsmBinaryAddPostupdate: {
            /* Two styles of syntax depending on whether this is at top-level or inside a memory reference expression. */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            if (suffix) {
                result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none);
                extra = ", " + unparseArmExpression(e->get_rhs(), labels, registers, arm_sign_plus);
            } else {
                /* Used by LDM* and STM* instructions outside memory reference expressions. RHS is unused. */
                result = unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none) + "!";
            }
            break;
        }
            
        case V_SgAsmBinarySubtractPostupdate: {
            /* Two styles of syntax depending on whether this is at top-level or inside a memory reference expression. */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            if (suffix) {
                result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none);
                extra = ", " + unparseArmExpression(e->get_rhs(), labels, registers, arm_sign_minus);
            } else {
                /* Used by LDM* and STM* instructions outside memory reference expressions. RHS is unused. */
                result += unparseArmExpression(e->get_lhs(), labels, registers, arm_sign_none) + "!";
            }
            break;
        }

        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            SgAsmExpression* addr = mr->get_address();
            switch (addr->variantT()) {
                case V_SgAsmDirectRegisterExpression:
                case V_SgAsmIndirectRegisterExpression:
                case V_SgAsmBinaryAdd:
                case V_SgAsmBinarySubtract:
                case V_SgAsmBinaryAddPreupdate:
                case V_SgAsmBinarySubtractPreupdate:
                case V_SgAsmBinaryAddPostupdate:
                case V_SgAsmBinarySubtractPostupdate:
                    break;
                default: ASSERT_not_reachable("bad addressing mode: " + stringifyVariantT(addr->variantT()));
            }

            std::string suffix;
            result += "[" + unparseArmExpression(addr, labels, registers, arm_sign_none, &suffix) + "]";
            result += suffix;
            break;
        }

        case V_SgAsmDirectRegisterExpression:
            result += unparseArmRegister(isSgAsmDirectRegisterExpression(expr), registers);
            break;
        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ve = isSgAsmIntegerValueExpression(expr);
            ASSERT_not_null(ve);
            uint64_t v = ve->get_absoluteValue();
            result += "#" + unparseArmSign(sign) + StringUtility::numberToString(v);
            if (expr->get_comment().empty() && v!=0 && labels && ve->get_significantBits()>8) {
                AsmUnparser::LabelMap::const_iterator li=labels->find(v);
                if (li!=labels->end())
                    result = StringUtility::appendAsmComment(result, li->second);
            }
            break;
        }

        default: {
            ASSERT_not_reachable("unhandled expression kind: " + expr->class_name());
        }
    }

    /* The extra data should be passed back up the call stack so it can be inserted into the ultimate return string. We can't
     * insert it here because the string can't be generated strictly left-to-right. If "suffix" is the null pointer then the
     * caller isn't expecting a suffix and we'll have to just do our best -- the result will not be valid ARM assembly. */
    if (extra.size()>0 && !suffix)
        result = "\"" + result + "\" and \"" + extra + "\"";
    if (suffix)
        *suffix = extra;

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

/** Returns a string for the part of the assembly instruction before the first operand. */
std::string unparseArmMnemonic(SgAsmArmInstruction *insn) {
    ASSERT_not_null(insn);
    std::string result = insn->get_mnemonic();
    std::string cond = unparseArmCondition(insn->get_condition());
    ASSERT_require(insn->get_positionOfConditionInMnemonic() >= 0);
    ASSERT_require((size_t)insn->get_positionOfConditionInMnemonic() <= result.size());
    result.insert(result.begin() + insn->get_positionOfConditionInMnemonic(), cond.begin(), cond.end());
    return result;
}

/** Returns the string representation of an instruction operand. Use unparseExpression() if possible. */
std::string unparseArmExpression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                 const RegisterDictionary *registers) {
    /* Find the instruction with which this expression is associated. */
    SgAsmArmInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmArmInstruction(node);
    }
    ASSERT_not_null(insn);

    if (insn->get_kind() == arm_b || insn->get_kind() == arm_bl) {
        ASSERT_require(insn->nOperands() == 1);
        ASSERT_require(insn->operand(0) == expr);
        SgAsmIntegerValueExpression* tgt = isSgAsmIntegerValueExpression(expr);
        ASSERT_not_null(tgt);
        return StringUtility::addrToString(tgt->get_value(), tgt->get_significantBits());
    } else {
        return unparseArmExpression(expr, labels, registers, arm_sign_none);
    }
}
