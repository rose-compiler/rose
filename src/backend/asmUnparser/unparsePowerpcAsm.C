/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

#include "rose.h"
#include <iomanip>
#include <boost/lexical_cast.hpp>

using namespace std;

/****************************************************
 * resolve expression
 ****************************************************/
string unparsePowerpcRegister(
         PowerpcRegisterClass cl,
         int reg,
         PowerpcConditionRegisterAccessGranularity gr) {
  switch (cl) {
    case powerpc_regclass_gpr: {
      return "r" + boost::lexical_cast<string>(reg);
    }
    case powerpc_regclass_fpr: {
      return "f" + boost::lexical_cast<string>(reg);
    }
    case powerpc_regclass_cr: {
      switch (gr) {
        case powerpc_condreggranularity_whole: {
          return "cr";
        }
        case powerpc_condreggranularity_field: {
          return "cr" + boost::lexical_cast<string>(reg);
        }
        case powerpc_condreggranularity_bit: {
          const char* crbitnames[4] = {"lt", "gt", "eq", "so"};
          return "cr" + boost::lexical_cast<string>(reg / 4) + "*4+" + crbitnames[reg % 4];
        }
        default: {
          ROSE_ASSERT (!"Bad condition register granularity");
        }
      }
    }
    case powerpc_regclass_fpscr: {
      return "fpscr";
    }
    case powerpc_regclass_spr: {
      // FIXME: add names
      return "spr" + boost::lexical_cast<string>(reg);
    }
    case powerpc_regclass_tbr: {
      // FIXME: add names
      return "tbr" + boost::lexical_cast<string>(reg);
    }
    case powerpc_regclass_msr: {
      return "msr";
    }
    case powerpc_regclass_sr: {
      return "sr" + boost::lexical_cast<string>(reg);
    }
    case powerpc_regclass_unknown: {
      return "unknown";
    }
    default:
      std::cerr << " Undefined Register - class=" << cl << " number=" << reg << std::endl;
      abort();
      break;
  }
}

string unparsePowerpcExpression(SgAsmExpression* expr) {
  string result = "";
  if (expr == NULL) return "BOGUS:NULL";
  switch (expr->variantT()) {
    case V_SgAsmBinaryAdd:
      result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs()) + " + " + unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs());
      break;
    case V_SgAsmMemoryReferenceExpression: {
      SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
      SgAsmExpression* addr = mr->get_address();
      switch (addr->variantT()) {
        case V_SgAsmBinaryAdd: {
          SgAsmBinaryAdd* a = isSgAsmBinaryAdd(addr);
          string lhs = unparsePowerpcExpression(a->get_lhs());
          if (isSgAsmWordValueExpression(a->get_rhs())) {
            result = boost::lexical_cast<string>((int)(int16_t)(isSgAsmWordValueExpression(a->get_rhs())->get_value())); // Sign-extend from 16 bits
            result += "(" + lhs + ")";
          } else {
            result = lhs + ", " + unparsePowerpcExpression(a->get_rhs());
          }
          break;
        }
        default: result = "(" + unparsePowerpcExpression(addr) + ")"; break;
      }
      break;
    }
    case V_SgAsmPowerpcRegisterReferenceExpression: {
      SgAsmPowerpcRegisterReferenceExpression* rr = isSgAsmPowerpcRegisterReferenceExpression(expr);
      result = unparsePowerpcRegister(rr->get_register_class(), rr->get_register_number(), rr->get_conditionRegisterGranularity());
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

string unparsePowerpcInstruction(SgAsmPowerpcInstruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  string result = insn->get_mnemonic();
  result += std::string((result.size() >= 7 ? 1 : 7 - result.size()), ' ');
  SgAsmOperandList* opList = insn->get_operandList();
  const SgAsmExpressionPtrList& operands = opList->get_operands();
  for (size_t i = 0; i < operands.size(); ++i) {
    if (i != 0) result += ", ";
    result += unparsePowerpcExpression(operands[i]);
  }
  return result;
}

string unparsePowerpcInstructionWithAddress(SgAsmPowerpcInstruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  return StringUtility::intToHex(insn->get_address()) + ':' + unparsePowerpcInstruction(insn);
}

