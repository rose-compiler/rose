// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "AsmUnparser_compat.h"

static std::string unparseArmRegister(
         SgAsmArmRegisterReferenceExpression::arm_register_enum code) {
  int c = (int)code;
  if (c >= SgAsmArmRegisterReferenceExpression::reg0 && c <= SgAsmArmRegisterReferenceExpression::reg15) {
    return "r" + StringUtility::numberToString(c - SgAsmArmRegisterReferenceExpression::reg0);
  } else if (c == SgAsmArmRegisterReferenceExpression::cpsr) {
    return "cpsr";
  } else if (c == SgAsmArmRegisterReferenceExpression::spsr) {
    return "spsr";
  } else if (c >= SgAsmArmRegisterReferenceExpression::cpsr_fields && c <= SgAsmArmRegisterReferenceExpression::cpsr_fields + 15) {
    std::string result = "cpsr_";
    uint8_t fields = c - SgAsmArmRegisterReferenceExpression::cpsr_fields;
    if (fields & 1) result += 'c';
    if (fields & 2) result += 'x';
    if (fields & 4) result += 's';
    if (fields & 8) result += 'f';
    return result;
  } else if (c >= SgAsmArmRegisterReferenceExpression::spsr_fields && c <= SgAsmArmRegisterReferenceExpression::spsr_fields + 15) {
    std::string result = "spsr_";
    uint8_t fields = c - SgAsmArmRegisterReferenceExpression::spsr_fields;
    if (fields & 1) result += 'c';
    if (fields & 2) result += 'x';
    if (fields & 4) result += 's';
    if (fields & 8) result += 'f';
    return result;
  } else {
    ROSE_ASSERT (!"Bad ARM register");

 // DQ (11/28/2009): MSVC warns about a path that does not have a return stmt.
    return "error in unparseArmRegister";
  }
}

static std::string unparseArmCondition(ArmInstructionCondition cond) { // Unparse as used for mnemonics
  switch (cond) {
    case arm_cond_unknown: return "***UNKNOWN***";
    case arm_cond_eq: return "eq";
    case arm_cond_ne: return "ne";
    case arm_cond_hs: return "hs";
    case arm_cond_lo: return "lo";
    case arm_cond_mi: return "mi";
    case arm_cond_pl: return "pl";
    case arm_cond_vs: return "vs";
    case arm_cond_vc: return "vc";
    case arm_cond_hi: return "hi";
    case arm_cond_ls: return "ls";
    case arm_cond_ge: return "ge";
    case arm_cond_lt: return "lt";
    case arm_cond_gt: return "gt";
    case arm_cond_le: return "le";
    case arm_cond_al: return "";
    case arm_cond_nv: return "nv";

	default:
       {
         ROSE_ASSERT (false);
      // DQ (11/28/2009): MSVC warns about a path that does not have a return stmt.
         return "error in unparseArmRegister";
	   }
  }
}

static std::string unparseArmSign(ArmSignForExpressionUnparsing sign) {
  switch (sign) {
    case arm_sign_none: return "";
    case arm_sign_plus: return "+";
    case arm_sign_minus: return "-";

	default:
       {
         ROSE_ASSERT (false);
      // DQ (11/28/2009): MSVC warns about a path that does not have a return stmt.
         return "error in unparseArmRegister";
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
static std::string unparseArmExpression(SgAsmExpression* expr, ArmSignForExpressionUnparsing sign, std::string *suffix=NULL) {
    std::string result, extra;
    if (!isSgAsmValueExpression(expr)) {
        result += unparseArmSign(sign);
    }
    switch (expr->variantT()) {
        case V_SgAsmBinaryMultiply:
            ROSE_ASSERT (isSgAsmByteValueExpression(isSgAsmBinaryExpression(expr)->get_rhs()));
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), arm_sign_none) + "*" +
                     StringUtility::numberToString(isSgAsmByteValueExpression(isSgAsmBinaryExpression(expr)->get_rhs()));
            break;
        case V_SgAsmBinaryLsl:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), arm_sign_none) + ", lsl " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), arm_sign_none);
            break;
        case V_SgAsmBinaryLsr:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), arm_sign_none) + ", lsr " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), arm_sign_none);
            break;
        case V_SgAsmBinaryAsr:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), arm_sign_none) + ", asr " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), arm_sign_none);
            break;
        case V_SgAsmBinaryRor:
            result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs(), arm_sign_none) + ", ror " +
                     unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs(), arm_sign_none);
            break;
        case V_SgAsmUnaryRrx:
            result = unparseArmExpression(isSgAsmUnaryExpression(expr)->get_operand(), arm_sign_none) + ", rrx";
            break;
        case V_SgAsmUnaryArmSpecialRegisterList:
            result += unparseArmExpression(isSgAsmUnaryExpression(expr)->get_operand(), arm_sign_none) + "^";
            break;
        case V_SgAsmExprListExp: {
            SgAsmExprListExp* el = isSgAsmExprListExp(expr);
            const std::vector<SgAsmExpression*>& exprs = el->get_expressions();
            result += "{";
            for (size_t i = 0; i < exprs.size(); ++i) {
                if (i != 0) result += ", ";
                result += unparseArmExpression(exprs[i], arm_sign_none);
            }
            result += "}";
            break;
        }


        case V_SgAsmBinaryAdd: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), arm_sign_plus);
            break;
        }

        case V_SgAsmBinarySubtract: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), arm_sign_minus);
            break;
        }

        case V_SgAsmBinaryAddPreupdate: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), arm_sign_plus);
            extra = "!";
            break;
        }
            
        case V_SgAsmBinarySubtractPreupdate: {
            /* This node cannot appear at the top of an ARM instruction operand tree */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            result += unparseArmExpression(e->get_lhs(), arm_sign_none) + ", " +
                      unparseArmExpression(e->get_rhs(), arm_sign_minus);
            extra = "!";
            break;
        }

        case V_SgAsmBinaryAddPostupdate: {
            /* Two styles of syntax depending on whether this is at top-level or inside a memory reference expression. */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            if (suffix) {
                result += unparseArmExpression(e->get_lhs(), arm_sign_none);
                extra = ", " + unparseArmExpression(e->get_rhs(), arm_sign_plus);
            } else {
                /* Used by LDM* and STM* instructions outside memory reference expressions. RHS is unused. */
                result = unparseArmExpression(e->get_lhs(), arm_sign_none) + "!";
            }
            break;
        }
            
        case V_SgAsmBinarySubtractPostupdate: {
            /* Two styles of syntax depending on whether this is at top-level or inside a memory reference expression. */
            SgAsmBinaryExpression *e = isSgAsmBinaryExpression(expr);
            if (suffix) {
                result += unparseArmExpression(e->get_lhs(), arm_sign_none);
                extra = ", " + unparseArmExpression(e->get_rhs(), arm_sign_minus);
            } else {
                /* Used by LDM* and STM* instructions outside memory reference expressions. RHS is unused. */
                result += unparseArmExpression(e->get_lhs(), arm_sign_none) + "!";
            }
            break;
        }

        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            SgAsmExpression* addr = mr->get_address();
            switch (addr->variantT()) {
                case V_SgAsmRegisterReferenceExpression:
                case V_SgAsmBinaryAdd:
                case V_SgAsmBinarySubtract:
                case V_SgAsmBinaryAddPreupdate:
                case V_SgAsmBinarySubtractPreupdate:
                case V_SgAsmBinaryAddPostupdate:
                case V_SgAsmBinarySubtractPostupdate:
                    break;
                default: ROSE_ASSERT (!"Bad addressing mode");
            }

            std::string suffix;
            result += "[" + unparseArmExpression(addr, arm_sign_none, &suffix) + "]";
            result += suffix;
            break;
        }

        case V_SgAsmArmRegisterReferenceExpression:
            result += unparseArmRegister(isSgAsmArmRegisterReferenceExpression(expr)->get_arm_register_code());
            break;
        case V_SgAsmByteValueExpression:
        case V_SgAsmWordValueExpression:
        case V_SgAsmDoubleWordValueExpression:
            result += "#" + unparseArmSign(sign) +
                      StringUtility::numberToString(SageInterface::getAsmConstant(isSgAsmValueExpression(expr)));
            break;
        default: {
            std::cerr << "Unhandled expression kind " << expr->class_name() << std::endl;
            ROSE_ASSERT (false);
        }
    }

    /* The extra data should be passed back up the call stack so it can be inserted into the ultimate return string. We can't
     * insert it here because the string can't be generated strictly left-to-right. If "suffix" is the null pointer then the
     * caller isn't expecting a suffix and we'll have to just do our best -- the result will not be valid ARM assembly. */
    if (extra.size()>0 && !suffix)
        result = "\"" + result + "\" and \"" + extra + "\"";
    if (suffix)
        *suffix = extra;

    if (expr->get_replacement() != "") {
        result += " <" + expr->get_replacement() + ">";
    }
    return result;
}

/** Returns a string for the part of the assembly instruction before the first operand. */
std::string unparseArmMnemonic(SgAsmArmInstruction *insn) {
    ROSE_ASSERT(insn!=NULL);
    std::string result = insn->get_mnemonic();
    std::string cond = unparseArmCondition(insn->get_condition());
    ROSE_ASSERT (insn->get_positionOfConditionInMnemonic() >= 0);
    ROSE_ASSERT ((size_t)insn->get_positionOfConditionInMnemonic() <= result.size());
    result.insert(result.begin() + insn->get_positionOfConditionInMnemonic(), cond.begin(), cond.end());
    return result;
}

/** Returns the string representation of an instruction operand. Use unparseExpress() if possible. */
std::string unparseArmExpression(SgAsmExpression *expr) {
    /* Find the instruction with which this expression is associated. */
    SgAsmArmInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmArmInstruction(node);
    }
    ROSE_ASSERT(insn!=NULL);

    if (insn->get_kind() == arm_b || insn->get_kind() == arm_bl) {
        ROSE_ASSERT(insn->get_operandList()->get_operands().size()==1);
        ROSE_ASSERT(insn->get_operandList()->get_operands()[0]==expr);
        SgAsmDoubleWordValueExpression* tgt = isSgAsmDoubleWordValueExpression(expr);
        ROSE_ASSERT(tgt);
        return StringUtility::intToHex(tgt->get_value());
    } else {
        return unparseArmExpression(expr, arm_sign_none);
    }
}

#if 0 /*use unparseInstruction() instead*/
/** Returns string representation of the instruction and all operands. */
string unparseArmInstruction(SgAsmArmInstruction* insn) {
    string result = unparseArmMnemonic(insn);
    result += '\t';
    SgAsmOperandList* opList = insn->get_operandList();
    const SgAsmExpressionPtrList& operands = opList->get_operands();
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i != 0) result += ", ";
        result += unparseArmExpression(operands[i]);
    }
    return result;
}
#endif

#if 0 /*use unparseInstructionWithAddress() instead*/
string unparseArmInstructionWithAddress(SgAsmArmInstruction* insn) {
  return StringUtility::intToHex(insn->get_address()) + '\t' + unparseArmInstruction(insn);
}
#endif
