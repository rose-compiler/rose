#include "rose.h"

using namespace std;

string unparseArmRegister(
         SgAsmArmRegisterReferenceExpression::arm_register_enum code) {
  int c = (int)code;
  if (c >= SgAsmArmRegisterReferenceExpression::reg0 && c <= SgAsmArmRegisterReferenceExpression::reg15) {
    return "r" + StringUtility::numberToString(c - SgAsmArmRegisterReferenceExpression::reg0);
  } else if (c == SgAsmArmRegisterReferenceExpression::cpsr) {
    return "cpsr";
  } else if (c == SgAsmArmRegisterReferenceExpression::spsr) {
    return "spsr";
  } else if (c >= SgAsmArmRegisterReferenceExpression::cpsr_fields && c <= SgAsmArmRegisterReferenceExpression::cpsr_fields + 15) {
    string result = "cpsr_";
    uint8_t fields = c - SgAsmArmRegisterReferenceExpression::cpsr_fields;
    if (fields & 1) result += 'c';
    if (fields & 2) result += 'x';
    if (fields & 4) result += 's';
    if (fields & 8) result += 'f';
    return result;
  } else if (c >= SgAsmArmRegisterReferenceExpression::spsr_fields && c <= SgAsmArmRegisterReferenceExpression::spsr_fields + 15) {
    string result = "spsr_";
    uint8_t fields = c - SgAsmArmRegisterReferenceExpression::spsr_fields;
    if (fields & 1) result += 'c';
    if (fields & 2) result += 'x';
    if (fields & 4) result += 's';
    if (fields & 8) result += 'f';
    return result;
  } else {
    ROSE_ASSERT (!"Bad ARM register");
  }
}

string unparseArmCondition(ArmInstructionCondition cond) { // Unparse as used for mnemonics
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
    default: ROSE_ASSERT (false);
  }
}

static string unparseArmSign(ArmSignForExpressionUnparsing sign) {
  switch (sign) {
    case arm_sign_none: return "";
    case arm_sign_plus: return "+";
    case arm_sign_minus: return "-";
    default: ROSE_ASSERT (false);
  }
}

string unparseArmExpression(SgAsmExpression* expr, ArmSignForExpressionUnparsing sign) {
  string result;
  if (!isSgAsmValueExpression(expr)) {
    result += unparseArmSign(sign);
  }
  switch (expr->variantT()) {
    case V_SgAsmBinaryMultiply:
      ROSE_ASSERT (isSgAsmByteValueExpression(isSgAsmBinaryExpression(expr)->get_rhs()));
      result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs()) + "*" + StringUtility::numberToString(isSgAsmByteValueExpression(isSgAsmBinaryExpression(expr)->get_rhs()));
      break;
    case V_SgAsmBinaryLsl:
      result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs()) + ", lsl " + unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmBinaryLsr:
      result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs()) + ", lsr " + unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmBinaryAsr:
      result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs()) + ", asr " + unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmBinaryRor:
      result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs()) + ", ror " + unparseArmExpression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmUnaryRrx:
      result = unparseArmExpression(isSgAsmUnaryExpression(expr)->get_operand()) + ", rrx";
      break;
    case V_SgAsmBinaryAddPostupdate: // These are only used outside memory refs in LDM* and STM* instructions
    case V_SgAsmBinarySubtractPostupdate:
      result = unparseArmExpression(isSgAsmBinaryExpression(expr)->get_lhs()) + "!";
      break;
    case V_SgAsmUnaryArmSpecialRegisterList:
      result += unparseArmExpression(isSgAsmUnaryExpression(expr)->get_operand()) + "^";
      break;
    case V_SgAsmExprListExp: {
      SgAsmExprListExp* el = isSgAsmExprListExp(expr);
      const vector<SgAsmExpression*>& exprs = el->get_expressions();
      result += "{";
      for (size_t i = 0; i < exprs.size(); ++i) {
        if (i != 0) result += ", ";
        result += unparseArmExpression(exprs[i]);
      }
      result += "}";
      break;
    }
    case V_SgAsmMemoryReferenceExpression: {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
      SgAsmExpression* addr = mr->get_address();
      switch (addr->variantT()) {
        case V_SgAsmRegisterReferenceExpression:
          result += "[" + unparseArmExpression(addr) + "]";
          break;
        case V_SgAsmBinaryAdd:
          result += "[" + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_lhs()) + ", " + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_rhs(), arm_sign_plus) + "]";
          break;
        case V_SgAsmBinarySubtract:
          result += "[" + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_lhs()) + ", " + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_rhs(), arm_sign_minus) + "]";
          break;
        case V_SgAsmBinaryAddPreupdate:
          result += "[" + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_lhs()) + ", " + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_rhs(), arm_sign_plus) + "]!";
          break;
        case V_SgAsmBinarySubtractPreupdate:
          result += "[" + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_lhs()) + ", " + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_rhs(), arm_sign_minus) + "]!";
          break;
        case V_SgAsmBinaryAddPostupdate:
          result += "[" + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_lhs()) + "], " + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_rhs(), arm_sign_plus);
          break;
        case V_SgAsmBinarySubtractPostupdate:
          result += "[" + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_lhs()) + "], " + unparseArmExpression(isSgAsmBinaryExpression(addr)->get_rhs(), arm_sign_minus);
          break;
        default: ROSE_ASSERT (!"Bad addressing mode");
      }
      break;
    }
    case V_SgAsmArmRegisterReferenceExpression:
      result += unparseArmRegister(isSgAsmArmRegisterReferenceExpression(expr)->get_arm_register_code());
      break;
    case V_SgAsmByteValueExpression:
      result += "#" + unparseArmSign(sign) + StringUtility::numberToString(isSgAsmByteValueExpression(expr)->get_value());
      break;
    case V_SgAsmWordValueExpression:
      result += "#" + unparseArmSign(sign) + StringUtility::numberToString(isSgAsmWordValueExpression(expr)->get_value());
      break;
    case V_SgAsmDoubleWordValueExpression:
      result += "#" + unparseArmSign(sign) + StringUtility::numberToString(isSgAsmDoubleWordValueExpression(expr)->get_value());
      break;
    default: {
      cerr << "Unhandled expression kind " << expr->class_name() << endl;
      ROSE_ASSERT (false);
    }
  }
  if (expr->get_replacement() != "") {
    result += " <" + expr->get_replacement() + ">";
  }
  return result;
}

string unparseArmInstruction(SgAsmArmInstruction* insn) {
  string mnemonic = insn->get_mnemonic();
  string cond = unparseArmCondition(insn->get_condition());
  ROSE_ASSERT (insn->get_positionOfConditionInMnemonic() >= 0);
  ROSE_ASSERT ((size_t)insn->get_positionOfConditionInMnemonic() <= mnemonic.size());
  mnemonic.insert(mnemonic.begin() + insn->get_positionOfConditionInMnemonic(), cond.begin(), cond.end());
  string result = mnemonic;
  result += '\t';
  SgAsmOperandList* opList = insn->get_operandList();
  const SgAsmExpressionPtrList& operands = opList->get_operands();
  if (insn->get_kind() == arm_b || insn->get_kind() == arm_bl) {
    ROSE_ASSERT (operands.size() == 1);
    SgAsmDoubleWordValueExpression* tgt = isSgAsmDoubleWordValueExpression(operands[0]);
    ROSE_ASSERT (tgt);
    result += StringUtility::intToHex(tgt->get_value());
  } else {
    for (size_t i = 0; i < operands.size(); ++i) {
      if (i != 0) result += ", ";
      result += unparseArmExpression(operands[i]);
    }
  }
  return result;
}

string unparseArmInstructionWithAddress(SgAsmArmInstruction* insn) {
  return StringUtility::intToHex(insn->get_address()) + '\t' + unparseArmInstruction(insn);
}
