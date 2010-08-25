/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
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
/****************************************************
 * resolve expression
 ****************************************************/
std::string unparseX86Register(X86RegisterClass cl, int reg, X86PositionInRegister pos) {
  switch (cl) {
    case x86_regclass_gpr: {
      static const char* regnames8l[16] = {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
      static const char* regnames8h[16] = {"ah", "ch", "dh", "bh", "", "", "", "", "", "", "", "", "", "", "", ""};
      static const char* regnames16[16] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
      static const char* regnames32[16] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
      static const char* regnames64[16] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
      ROSE_ASSERT (reg >= 0 && reg <= 15);
      switch (pos) {
        case x86_regpos_low_byte: return regnames8l[reg];
        case x86_regpos_high_byte: ROSE_ASSERT (reg <= 3); return regnames8h[reg];
        case x86_regpos_word: return regnames16[reg];
        case x86_regpos_dword: return regnames32[reg];
        case x86_regpos_qword: return regnames64[reg];
        case x86_regpos_unknown: return regnames64[reg];
        case x86_regpos_all: return regnames64[reg];
        default: ROSE_ASSERT (!"Bad position in register");
      }
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
      switch (pos) {
        case x86_regpos_word: return "ip";
        case x86_regpos_dword: return "eip";
        case x86_regpos_qword: return "rip";
        case x86_regpos_unknown: return "rip";
        case x86_regpos_all: return "rip";
        default: ROSE_ASSERT (!"Bad position in register");
      }
    } 
    case x86_regclass_mm: {
      return "mm" + StringUtility::numberToString(reg);
    }
    case x86_regclass_xmm: {
      return "xmm" + StringUtility::numberToString(reg);
    }
    case x86_regclass_cr: {
      return "cr" + StringUtility::numberToString(reg);
    }
    case x86_regclass_dr: {
      return "dr" + StringUtility::numberToString(reg);
    }
    case x86_regclass_flags: {
      switch (pos) {
        case x86_regpos_word: return "flags";
        case x86_regpos_dword: return "eflags";
        case x86_regpos_qword: return "rflags";
        case x86_regpos_unknown: return "rflags";
        case x86_regpos_all: return "rflags";
        default: ROSE_ASSERT (!"Bad position in register");
      }
    }
    case x86_regclass_unknown: {
      return "unknown";
    }
    default:
      std::cerr << " Undefined Register - class=" << regclassToString(cl) << " number=" << reg << std::endl;
      abort();
      // DQ (11/29/2009): Avoid MSVC warning.
      return "error in unparseX86Register()";
      break;
  }
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
            result = unparseX86Register(rr->get_register_class(), rr->get_register_number(), rr->get_position_in_register());
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
