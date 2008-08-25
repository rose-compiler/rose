#include "rose.h"

using namespace std;

string unparseInstruction(SgAsmInstruction* insn) {
  switch (insn->variantT()) {
    case V_SgAsmx86Instruction: return unparseX86Instruction(isSgAsmx86Instruction(insn));
    case V_SgAsmArmInstruction: return unparseArmInstruction(isSgAsmArmInstruction(insn));
    default: cerr << "Unhandled variant " << insn->class_name() << " in unparseInstruction" << endl; abort();
  }
}

string unparseInstructionWithAddress(SgAsmInstruction* insn) {
  switch (insn->variantT()) {
    case V_SgAsmx86Instruction: return unparseX86InstructionWithAddress(isSgAsmx86Instruction(insn));
    case V_SgAsmArmInstruction: return unparseArmInstructionWithAddress(isSgAsmArmInstruction(insn));
    default: cerr << "Unhandled variant " << insn->class_name() << " in unparseInstructionWithAddress" << endl; abort();
  }
}

// DQ (8/23/2008): I think this should take an SgAsmStatement
// string unparseAsmStatement(SgAsmNode* stmt)
string
unparseAsmStatement(SgAsmStatement* stmt)
{
  ROSE_ASSERT (stmt != NULL);
  string result;
  if (stmt->get_comment().empty() == false)
       result = "/* " + stmt->get_comment() + " */\n";

#if 0
  string addressString = "/* Address: " + StringUtility::intToHex(stmt->get_address()) + " */";
  printf ("comment empty = %s addressString = %s \n",stmt->get_comment().empty() ? "true" : "false", addressString.c_str());
#endif

  switch (stmt->variantT()) {
    case V_SgAsmx86Instruction: return result + unparseX86InstructionWithAddress(isSgAsmx86Instruction(stmt)) + '\n';
    case V_SgAsmArmInstruction: return result + unparseArmInstructionWithAddress(isSgAsmArmInstruction(stmt)) + '\n';
    case V_SgAsmBlock: {
      SgAsmBlock* blk = isSgAsmBlock(stmt);
      result = result + "/* Block " + StringUtility::intToHex(blk->get_address()) + " */\n";
      for (size_t i = 0; i < blk->get_statementList().size(); ++i) {
        result += unparseAsmStatement(blk->get_statementList()[i]);
      }
      return result;
    }
    case V_SgAsmFunctionDeclaration: {
      SgAsmFunctionDeclaration* blk = isSgAsmFunctionDeclaration(stmt);
      result = result + "/* Function " + blk->get_name() + " at " + StringUtility::intToHex(blk->get_address()) + " */\n";
      for (size_t i = 0; i < blk->get_statementList().size(); ++i) {
        result += unparseAsmStatement(blk->get_statementList()[i]);
      }
      return result;
    }
    default: {
      cerr << "Unhandled variant " << stmt->class_name() << " in unparseX86Statement" << endl;
      ROSE_ASSERT (false);
    }
  }
}

// void unparseAsmStatementToFile(const string& filename, SgAsmNode* stmt) {
void unparseAsmStatementToFile(const string& filename, SgAsmStatement* stmt) {
  ROSE_ASSERT (stmt != NULL);
  ofstream of(filename.c_str());
  of << unparseAsmStatement(stmt);
}
