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

string unparseAsmStatement(SgAsmNode* stmt) {
  ROSE_ASSERT (isSgAsmStatement(stmt));
  switch (stmt->variantT()) {
    case V_SgAsmx86Instruction: return unparseX86InstructionWithAddress(isSgAsmx86Instruction(stmt)) + '\n';
    case V_SgAsmArmInstruction: return unparseArmInstructionWithAddress(isSgAsmArmInstruction(stmt)) + '\n';
    case V_SgAsmBlock: {
      SgAsmBlock* blk = isSgAsmBlock(stmt);
      string result = "/* Block " + StringUtility::intToHex(blk->get_address()) + " */\n";
      for (size_t i = 0; i < blk->get_statementList().size(); ++i) {
        result += unparseAsmStatement(blk->get_statementList()[i]);
      }
      return result;
    }
    case V_SgAsmFunctionDeclaration: {
      SgAsmFunctionDeclaration* blk = isSgAsmFunctionDeclaration(stmt);
      string result = "/* Function " + blk->get_name() + " at " + StringUtility::intToHex(blk->get_address()) + " */\n";
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

void unparseAsmStatementToFile(const string& filename, SgAsmNode* stmt) {
  ROSE_ASSERT (isSgAsmStatement(stmt));
  ofstream of(filename.c_str());
  of << unparseAsmStatement(stmt);
}
