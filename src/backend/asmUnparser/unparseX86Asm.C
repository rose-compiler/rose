#include "sage3basic.h"
#include "Registers.h"
#include <iomanip>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string unparseX86Mnemonic(SgAsmx86Instruction *insn) {
    ROSE_ASSERT(insn!=NULL);
    std::string result = insn->get_mnemonic();
    switch (insn->get_branchPrediction()) {
        case x86_branch_prediction_none: break;
        case x86_branch_prediction_taken: result += ",pt"; break;
        case x86_branch_prediction_not_taken: result += ",pn"; break;
        default: ROSE_ASSERT (!"Bad branch prediction");
    }
    return result;
}

/** Returns the name of an X86 register.
 *
 *  We use the amd64 architecture because, since it's backward compatible with the 8086, it contains definitions for all the
 *  registers from older architectures. */
std::string unparseX86Register(const RegisterDescriptor &reg) {
    const RegisterDictionary *dict = RegisterDictionary::amd64();
    std::string name = dict->lookup(reg);
    if (name.empty()) {
        std::cerr <<"unparseX86Register(" <<reg <<"): register descriptor not found in dictionary.\n";
        //std::cerr <<dict;
        ROSE_ASSERT(!"register descriptor not found in dictionary");
    }
    return name;
}

static std::string x86TypeToPtrName(SgAsmType* ty) {
  ROSE_ASSERT(ty != NULL);
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
    default:
       {
         std::cerr << "x86TypeToPtrName: Bad class " << ty->class_name() << std::endl;
		 ROSE_ASSERT(false);
      // DQ (11/29/2009): Avoid MSVC warning.
         return "error in x86TypeToPtrName()";
       }
  }
}


std::string unparseX86Expression(SgAsmExpression *expr, bool leaMode) {
    std::string result = "";
    if (expr == NULL) return "BOGUS:NULL";
    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), false) + " + " +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), false);
            break;
        case V_SgAsmBinarySubtract:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), false) + " - " +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), false);
            break;
        case V_SgAsmBinaryMultiply:
            result = unparseX86Expression(isSgAsmBinaryExpression(expr)->get_lhs(), false) + "*" +
                     unparseX86Expression(isSgAsmBinaryExpression(expr)->get_rhs(), false);
            break;
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            if (!leaMode) {
                result += x86TypeToPtrName(mr->get_type()) + " PTR " +
                          (mr->get_segment() ? unparseX86Expression(mr->get_segment(), false) + ":" : "");
            }
            result += "[" + unparseX86Expression(mr->get_address(), false) + "]";
            break;
        }
        case V_SgAsmx86RegisterReferenceExpression: {
            SgAsmx86RegisterReferenceExpression* rr = isSgAsmx86RegisterReferenceExpression(expr);
            result = unparseX86Register(rr->get_descriptor());
            break;
        }
        case V_SgAsmByteValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            sprintf(buf, "0x%02"PRIx64, v);
            if (v & 0x80)
                sprintf(buf+strlen(buf), "(-0x%02"PRIx64")", (~v+1) & 0xff);
            result = buf;
            break;
        }
        case V_SgAsmWordValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            sprintf(buf, "0x%04"PRIx64, v);
            if (v & 0x8000)
                sprintf(buf+strlen(buf), "(-0x%04"PRIx64")", (~v+1) & 0xffff);
            result = buf;
            break;
        }
        case V_SgAsmDoubleWordValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            sprintf(buf, "0x%08"PRIx64, v);
            if (v & 0x80000000)
                sprintf(buf+strlen(buf), "(-0x%08"PRIx64")", (~v+1) & 0xffffffff);
            result = buf;
            break;
        }
        case V_SgAsmQuadWordValueExpression: {
            char buf[64];
            uint64_t v = SageInterface::getAsmConstant(isSgAsmValueExpression(expr));
            sprintf(buf, "0x%016"PRIx64, v);
            if (v & ((uint64_t)1<<63))
                sprintf(buf+strlen(buf), "(-0x%016"PRIx64")", (~v+1));
            result = buf;
            break;
        }
        default: {
            std::cerr << "Unhandled expression kind " << expr->class_name() << std::endl;
            ROSE_ASSERT (false);
        }
    }
    if (expr->get_replacement() != "") {
        result += " <" + expr->get_replacement() + ">";
    }
#if 0
    if (expr->get_bit_size()>0) {
        result += " <@" + StringUtility::numberToString(expr->get_bit_offset()) +
                  "+" + StringUtility::numberToString(expr->get_bit_size()) + ">";
    }
#endif
    return result;
}

/** Returns a string containing the specified operand. */
std::string unparseX86Expression(SgAsmExpression *expr) {
    /* Find the instruction with which this expression is associated. */
    SgAsmx86Instruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmx86Instruction(node);
    }
    ROSE_ASSERT(insn!=NULL);
    return unparseX86Expression(expr, insn->get_kind()==x86_lea);
}




# if 0 /*use unparseInstruction() instead */
static string unparseX86Instruction(SgAsmx86Instruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  string result = unparseX86Mnemonic(insn);
  result += std::string((result.size() >= 7 ? 1 : 7 - result.size()), ' ');
  SgAsmOperandList* opList = insn->get_operandList();
  const SgAsmExpressionPtrList& operands = opList->get_operands();
  for (size_t i = 0; i < operands.size(); ++i) {
    if (i != 0) result += ", ";
    result += unparseX86Expression(insn, operands[i]);
  }
  if (insn->get_comment()!="")
    result+="  <"+insn->get_comment()+">";

  return result;
}
#endif

#if 0 /*use unparseInstructionWithAddress() instead */
string unparseX86InstructionWithAddress(SgAsmx86Instruction* insn) {
  if (insn == NULL) return "BOGUS:NULL";
  return StringUtility::intToHex(insn->get_address()) + ':' + unparseX86Instruction(insn);
}
#endif
