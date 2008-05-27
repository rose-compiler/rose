/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

#include "rose.h"
#include <iomanip>

using namespace std;

static string getNameForPartialRegister(const string& fullRegName, X86PositionInRegister pos) {
  ROSE_ASSERT (fullRegName.size() >= 2 && fullRegName[0] == 'r');
  enum RegisterClass {numbered, sidibpspip, axbxcxdx};
  RegisterClass regClass;
  if (isdigit(fullRegName[1])) {
    regClass = numbered;
  } else if (fullRegName[fullRegName.size() - 1] != 'x') { // Includes rflags
    regClass = sidibpspip;
  } else {
    regClass = axbxcxdx;
  }
  switch (pos) {
    case x86_regpos_low_byte: {
      switch (regClass) {
        case numbered: return fullRegName + "b";
        case sidibpspip: ROSE_ASSERT (fullRegName != "rip"); return fullRegName.substr(1, 2) + "l";
        case axbxcxdx: return fullRegName.substr(1, 1) + "l";
      }
    }
    case x86_regpos_high_byte: {
      ROSE_ASSERT (regClass == axbxcxdx);
      return fullRegName.substr(1, 1) + "h";
    }
    case x86_regpos_word: {
      switch (regClass) {
        case numbered: return fullRegName + "w";
        case sidibpspip: case axbxcxdx: return fullRegName.substr(1, 2);
      }
    }
    case x86_regpos_dword: {
      switch (regClass) {
        case numbered: return fullRegName + "d";
        case sidibpspip: case axbxcxdx: return "e" + fullRegName.substr(1, 2);
      }
    }
    case x86_regpos_qword:
    case x86_regpos_unknown:
    case x86_regpos_all: {
      return fullRegName;
    }
    default: ROSE_ASSERT (!"Bad position in register");
  }
}


/****************************************************
 * resolve expression
 ****************************************************/
string unparseX86Register(
         X86RegisterClass cl,
         int reg,
         X86PositionInRegister pos) {
  switch (cl) {
    case x86_regclass_gpr: {
      static const char* regnames[16] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
      ROSE_ASSERT (reg >= 0 && reg <= 15);
      return getNameForPartialRegister(regnames[reg], pos);
    }
    case x86_regclass_segment: {
      ROSE_ASSERT (reg >= 0 && reg <= 5);
      static const char* segregnames[6] = {"es", "cs", "ss", "ds", "fs", "gs"};
      return segregnames[reg];
    }
    case x86_regclass_st_top: {
      return "st";
    }
    case x86_regclass_st: {
      return "st(" + StringUtility::numberToString(reg) + ")";
    }
    case x86_regclass_ip: {
      return getNameForPartialRegister("rip", pos);
    } 
    case x86_regclass_mm: {
      return "mm(" + StringUtility::numberToString(reg) + ")";
    }
    case x86_regclass_xmm: {
      return "xmm(" + StringUtility::numberToString(reg) + ")";
    }
    case x86_regclass_cr: {
      return "cr" + StringUtility::numberToString(reg);
    }
    case x86_regclass_dr: {
      return "dr" + StringUtility::numberToString(reg);
    }
    case x86_regclass_flags: {
      return getNameForPartialRegister("rflags", pos);
    }
    case x86_regclass_unknown: {
      return "unknown";
    }
    default:
      std::cerr << " Undefined Register - class=" << cl << " number=" << reg << std::endl;
      abort();
      break;
  }
}

static string x86TypeToPtrName(SgAsmType* ty);

string unparseX86Expression(SgAsmExpression* expr, bool leaMode) {
  string result = "";
  if (expr == NULL) return "BOGUS:NULL";
  switch (expr->variantT()) {
    case V_SgAsmBinaryAdd:
      result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs()) + " + " + unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmBinarySubtract:
      result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs()) + " - " + unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmBinaryMultiply:
      result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs()) + "*" + unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmMemoryReferenceExpression: {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
      if (!leaMode) {
        result += x86TypeToPtrName(mr->get_type()) + " PTR " + (mr->get_segment() ? unparseX86Expression(mr->get_segment()) + ":" : "");
      }
      result += "[" + unparseX86Expression(mr->get_address()) + "]";
      break;
    }
    case V_SgAsmx86RegisterReferenceExpression: {
      SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(expr);
      result = unparseX86Register(rr->get_register_class(), rr->get_register_number(), rr->get_position_in_register());
      break;
    }
    case V_SgAsmByteValueExpression: result = StringUtility::intToHex(isSgAsmByteValueExpression(expr)->get_value());
                                     break;
    case V_SgAsmWordValueExpression: result = StringUtility::intToHex(isSgAsmWordValueExpression(expr)->get_value());
                                     break;
    case V_SgAsmDoubleWordValueExpression: result = StringUtility::intToHex(isSgAsmDoubleWordValueExpression(expr)->get_value());
                                           break;
    case V_SgAsmQuadWordValueExpression: result = StringUtility::intToHex(isSgAsmQuadWordValueExpression(expr)->get_value());
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

string unparseX86Instruction(SgAsmx86Instruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  string result = insn->get_mnemonic();
  switch (insn->get_branchPrediction()) {
    case x86_branch_prediction_none: break;
    case x86_branch_prediction_taken: result += ",pt"; break;
    case x86_branch_prediction_not_taken: result += ",pn"; break;
    default: ROSE_ASSERT (!"Bad branch prediction");
  }
  result += std::string((result.size() >= 7 ? 1 : 7 - result.size()), ' ');
  SgAsmOperandList* opList = insn->get_operandList();
  const SgAsmExpressionPtrList& operands = opList->get_operands();
  for (size_t i = 0; i < operands.size(); ++i) {
    if (i != 0) result += ", ";
    result += unparseX86Expression(operands[i], (insn->get_kind() == x86_lea));
  }
  return result;
}

string unparseX86InstructionWithAddress(SgAsmx86Instruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  return StringUtility::intToHex(insn->get_address()) + ':' + unparseX86Instruction(insn);
}

static string x86TypeToPtrName(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return "BYTE";
    case V_SgAsmTypeWord: return "WORD";
    case V_SgAsmTypeDoubleWord: return "DWORD";
    case V_SgAsmTypeQuadWord: return "QWORD";
    case V_SgAsmTypeDoubleQuadWord: return "DQWORD";
    case V_SgAsmTypeSingleFloat: return "FLOAT";
    case V_SgAsmTypeDoubleFloat: return "DOUBLE";
    case V_SgAsmType80bitFloat: return "LDOUBLE";
    case V_SgAsmTypeVector: {
      SgAsmTypeVector* v = isSgAsmTypeVector(ty);
      return "V" + StringUtility::numberToString(v->get_elementCount()) + x86TypeToPtrName(v->get_elementType());
    }
    default: {std::cerr << "x86TypeToPtrName: Bad class " << ty->class_name() << std::endl; ROSE_ASSERT(false);}
  }
}

